//#include<Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include <ArduinoJson.h>
#include <Preferences.h>
#include <string>

//const-----------
const char* DeviceId="bb505cb3-ee8e-4c70-9b11-bdba7c210bb0";

const uint8_t LIGHT_RELAY = 27;
const uint8_t SENSOR_RELAY = 26;
const uint8_t PUMP_RELAY = 25;

const uint8_t DHT_DATA = 32;
const uint8_t LIGHT_DATA = 33;
const uint8_t WATERLEVEL_DATA = 35;

const uint8_t LED = 2;

const char* bale = "https://tapi.bale.ai/botXzzpuaR1853wr4vFFqewH6NcVfSHEYadh2xPLinI/sendMessage";
const char* host = "http://192.168.43.94:5266/api/Micro/SendData";
const char* getconfig = "http://192.168.43.94:5266/api/Micro/GetConfig";

const int m_ms=60 * 1000;
const int m_Ms=60 * 1000000;

const float LIGHT_INCREMENT = 0.25;

//handle----------
TimerHandle_t PumpTimer;
TimerHandle_t SensorTimer;
TaskHandle_t WifiTaskHandle;
TaskHandle_t HttpClientHandle;
TaskHandle_t LightControlHandle;
TaskHandle_t SettingHandle;

//struct---------

typedef struct
{
  float light;
  float humidity;
  float temperature;
  float waterlevel;
}SensorData;


typedef struct
{
 std::string ConfigVersion="default";
 int PUMP_ON_DURATION = 3;      // 3 minutes 
 int PUMP_OFF_DURATION = 3;    // 3 minutes
 int SENSOR_CYCLE = 10;         // 10 minutes
 std::string ssid = "DELSA"; 
 std::string password = "2283524938";
 int LIGHT_THRESHOLD = 30;//percent
}Config;

typedef struct {
  std::string data;  
} ResponseData;

// globals ----------
Preferences prefs;
DHT dht(DHT_DATA,DHT11);
Config config;
bool PumpIsOn = false;
bool LightIsOn =false;

//Queue-------
QueueHandle_t SensorQueue;
QueueHandle_t LightQueue;
QueueHandle_t HourQueue;
QueueHandle_t ResponseQueue;

//eventgroup---------


//Pump callback----------------
void PumpCallback(TimerHandle_t xTimer)
{
  Serial.println("in pump callback");
  if(PumpIsOn)
  {
    Serial.println("make pump off");
    digitalWrite(PUMP_RELAY,HIGH);//make pump off
    PumpIsOn=false;
    xTimerChangePeriod(PumpTimer, pdMS_TO_TICKS(config.PUMP_OFF_DURATION*m_ms), 0);
  }
  else 
  {
    Serial.println("make pump on");
    digitalWrite(PUMP_RELAY,LOW);//make pump on
    PumpIsOn=true;
    xTimerChangePeriod(PumpTimer, pdMS_TO_TICKS(config.PUMP_ON_DURATION*m_ms), 0);
  }
}

//Sensor Callback-----------

void SensorCallback(TimerHandle_t xTimer) 
{
  Serial.println("in Sensor callback");
  SensorData data;

   digitalWrite(SENSOR_RELAY,LOW);//make dht and light sensor on
   delay(1000);
   data.humidity=dht.readHumidity() ;
   delay(500);
   data.temperature=dht.readTemperature();
   delay(500);
   int analogLight=analogRead(LIGHT_DATA);
   delay(500);
   data.light = map(analogLight, 0, 4095, 0, 100);
   digitalWrite(SENSOR_RELAY,HIGH);//make dht and light sensor off
   delay(500);
   int analogwaterlevel=analogRead(WATERLEVEL_DATA);
   delay(500);
   data.waterlevel = map(analogwaterlevel, 0, 4095, 0, 100);
   delay(100);
   digitalWrite(SENSOR_RELAY,HIGH);//make dht and light sensor off
   delay(100);
 if(xQueueSend(SensorQueue, &data, portMAX_DELAY) == pdTRUE )
 {
  xTaskNotifyGive(WifiTaskHandle);  // unblock wifi task
 }
  xQueueSend(LightQueue, &data.light, portMAX_DELAY);

  xTimerChangePeriod(SensorTimer, pdMS_TO_TICKS(config.SENSOR_CYCLE*m_ms), 0);

}


//Wifi Task---------------
void WifiTask(void *pvParameters) 
{
  Serial.println("in wifi task");
  for(;;)
  {
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
     Serial.println("in wifi task : after notification");
     if(WiFi.status() != WL_CONNECTED)
      {
        WiFi.disconnect();//to ensure
        const char* ssid=config.ssid.c_str();
        const char* password=config.password.c_str();
        Serial.println("connecting to wifi");
        Serial.printf("ssid and password is:%s \n %s",ssid,password);
        WiFi.begin(ssid,password);
        vTaskDelay(pdMS_TO_TICKS(1000));

       for(int i=0;i<10;i++)
        {
          if(WiFi.status() == WL_CONNECTED)
           break;
          vTaskDelay(pdMS_TO_TICKS(1000));
          Serial.print(".");
        }
       
      }
    if(WiFi.status() == WL_CONNECTED)
      {
      Serial.println("\n wifi connected");
      
      digitalWrite(LED, LOW);//MAKE LED OFF
    
      xTaskNotifyGive(HttpClientHandle);
      }
    else
      {
        Serial.println("\n connection fail");
        digitalWrite(LED, HIGH);//MAKE LED ON
        int data=-1;
        xQueueSend(HourQueue,&data, 0);
      }
  }

}

//httpclient Task----------
void HttpClient(void *pvParameters) 
{
  Serial.println(" httpclient task");
  SensorData data;
  ResponseData response;
  for(;;)
  {
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
     Serial.println(" httpclient task : after notification");
      if (WiFi.status() != WL_CONNECTED) 
      {
      Serial.println("httpclient task :WiFi not connected!");
      continue; 
     }
      xQueueReceive(SensorQueue, & data, 0);

          int ResponseCode = -2;
          HTTPClient http2;
          delay(100);
         http2.begin(host);
          vTaskDelay(pdMS_TO_TICKS(1000));
          http2.addHeader("Content-Type","application/json");
          StaticJsonDocument<256> doc;
            doc["DeviceId"] = DeviceId;
            doc["humidity"] = data.humidity;
            doc["temperature"] = data.temperature;
            doc["light"] = data.light;
            doc["waterlevel"]=data.waterlevel;
            doc["LightIsOn"] = LightIsOn;
            String payload2;
            serializeJson(doc,payload2);
            Serial.println(payload2);
          for (int retry = 0; retry < 5; retry++) 
          {
            ResponseCode = http2.POST(payload2);
            vTaskDelay(pdMS_TO_TICKS(1000));
            Serial.printf("Attempt %d: HTTP Code %d\n", retry + 1, ResponseCode);
            if (ResponseCode == 200)
               break; 
          }
          if(ResponseCode == 200)
          {
          
            String hour=http2.getString();
            Serial.printf("httpclient task -> first response : %s\n",hour);
            StaticJsonDocument<JSON_OBJECT_SIZE(2) + 300> doc;
            DeserializationError error=deserializeJson(doc,hour);
            delay(100);
            int data=-1;
            if(!error)
            {
              data=doc["hour"].as<int>();
              xQueueSend(HourQueue, &data, 0);
            } 
            else
            {
              Serial.println("httpclient -> first response ->json error");
              xQueueSend(HourQueue,&data, 0);//to unblock light task and use current light data so in next try we have correct hour and light data
            }
           
          }
          else
          {
            Serial.println("httpclient -> fail to send first request");
            int data=-1;
            xQueueSend(HourQueue,&data, 0);//to unblock light task and use current light data so in next try we have correct hour and light data
          }

          
          http2.end();
          vTaskDelay(pdMS_TO_TICKS(1000));

          ResponseCode = -2;
          HTTPClient http3;
          Serial.println("httpclient task -> send request to getconfig \n");
          http3.begin(getconfig);
          vTaskDelay(pdMS_TO_TICKS(1000));
          http3.addHeader("Content-Type","application/json");
          StaticJsonDocument<128> doc_b;
            doc_b["DeviceId"] = DeviceId;
            doc_b["ConfigVersion"] = config.ConfigVersion;
            String payload3;
            serializeJson(doc_b,payload3);
          for (int retry = 0; retry < 5; retry++) 
          {
            ResponseCode = http3.POST(payload3);
            Serial.printf("Attempt %d: HTTP Code %d\n", retry + 1, ResponseCode);
            if (ResponseCode == 200)
               break; 
            vTaskDelay(pdMS_TO_TICKS(1000)); 
          }
          if(ResponseCode == 200)
          {
            String input=http3.getString();
            response.data= input.c_str();
            Serial.printf("httpclient task -> second response : %s\n",response.data);
            xQueueSend(ResponseQueue, &response, portMAX_DELAY);
            xTaskNotifyGive(SettingHandle);
          }
          http3.end();
          vTaskDelay(pdMS_TO_TICKS(1000));
  }

}
//light control task-----
void LightControlTask(void *pvParameters)
{
  Serial.println("light control");
  float Light;
  int CurrentHour=-1;
  bool IsDay=false;
for(;;)
{
  xQueueReceive(HourQueue, &CurrentHour, portMAX_DELAY);
  xQueueReceive(LightQueue, &Light, portMAX_DELAY);
  Serial.println("light control:light is " + String(Light) + "hour is:" + String(CurrentHour));
  IsDay = CurrentHour >= 5 && CurrentHour < 19; 
  if (IsDay)
  {
    if(Light<config.LIGHT_THRESHOLD)
    {
        if(!LightIsOn)
        {
          Serial.println("light control: turn light on");
          digitalWrite(LIGHT_RELAY,LOW);//turn light on
          delay(100);
          LightIsOn=true;
        }
    }
    else
    {
      if(LightIsOn){
          Serial.println("light control:turn light off (light>threshold)");
          digitalWrite(LIGHT_RELAY,HIGH);//turn light off
          delay(100);
          LightIsOn=false;
        }
    }
  }
  else 
  {
  Serial.println("light control:not in day");
  }

  
}

}

//--------
void Setting(void *pvParameters)
{
  Serial.println("setting task");
  ResponseData response;
  bool IsChanged;
  StaticJsonDocument<JSON_OBJECT_SIZE(10) + 300> doc;
  DeserializationError error;
  for(;;)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    Serial.println("setting task -> get notification\n");
    xQueueReceive(ResponseQueue, & response, 0);
    delay(100);
    const char* command=response.data.c_str();
    delay(100);
    Serial.printf("setting task -> response : %s\n",command);
    delay(100);
    doc.clear();
    delay(100);
    error= deserializeJson(doc,command);
    delay(100);
    if(error)
    {
      Serial.print("JSON error: ");
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    } 
      IsChanged=doc["ischanged"];
      if(!IsChanged)
      {
        Serial.println("No change in config");
        continue;
      }
         Serial.println("setting -> config changes");
         
        if(!prefs.begin("config", false))
        {
          Serial.println("setting task ->Failed to init preferences");
          continue;

        }
         delay(100);
         const char* configversion = doc["configversion"].as<const char*>();
         Serial.printf("setting task -> after deserialize - configversion is %s\n",configversion);
         prefs.putString("ConfigVersion",configversion);
         delay(100); 
         int pump_on_duration = doc["pump_on_duration"].as<int>();
         Serial.printf("setting task -> after deserialize - pump_on_duration is %d\n",pump_on_duration);
         prefs.putInt("PUMPON",pump_on_duration);
        delay(100);
         int pump_off_duration = doc["pump_off_duration"].as<int>();
         Serial.printf("setting task -> after deserialize - pump_off_duration is %d\n",pump_off_duration);
         prefs.putInt("PUMPOFF", pump_off_duration);
          delay(100);
         int sensor_cycle = doc["sensor_cycle"].as<int>();
         Serial.printf("setting task -> after deserialize - sensor_cycle is %d\n",sensor_cycle);
         prefs.putInt("SENSORCYCLE", sensor_cycle);
          delay(100);
         const char* ssid = doc["ssid"].as<const char*>();
         Serial.printf("setting task -> after deserialize - ssid is %s\n",ssid);
         prefs.putString("ssid",ssid);
          delay(100);
         const char* password = doc["password"].as<const char*>();
         Serial.printf("setting task -> after deserialize - password is %s\n",password);
         prefs.putString("password",password);
         delay(100);
         int light_threshold = doc["light_threshold"].as<int>();
         Serial.printf("setting task -> after deserialize - light_threshold is %d\n",light_threshold);
         prefs.putInt("LIGHT",light_threshold);
          delay(100);
         
         
         prefs.end();
            
         delay(100);
         
         LoadConfig();
         
         Serial.println("setting -> config structer updated");
      
  }

}

void LoadConfig() 
{
Serial.println("load config->start to load config ");
if(!prefs.begin("config", true)) { 
    Serial.println("load config -> Failed to init preferences");
    return;
  }

Serial.printf("load config");
int pump_on_duration=prefs.getInt("PUMPON");
delay(100);
if(pump_on_duration != 0)
config.PUMP_ON_DURATION=pump_on_duration;


int pump_off_duration=prefs.getInt("PUMPOFF");
delay(100);
if(pump_off_duration != 0)
config.PUMP_OFF_DURATION=pump_off_duration;

int sensor_cycle=prefs.getInt("SENSORCYCLE");
delay(100);
if(sensor_cycle != 0)
config.SENSOR_CYCLE=sensor_cycle;

int light_threshold=prefs.getInt("LIGHT");
delay(100);
if(light_threshold != 0)
config.LIGHT_THRESHOLD=light_threshold;


String ssid =prefs.getString("ssid");
delay(100);
if(!ssid.isEmpty())
config.ssid=ssid.c_str();

String password=prefs.getString("password");
delay(100);
if(!password.isEmpty())
config.password=password.c_str();

String configversion=prefs.getString("ConfigVersion");
delay(100);
if(!configversion.isEmpty())
config.ConfigVersion=configversion.c_str();

prefs.end();

delay(100);
Serial.println("load config -> config loaded.config is:");
Serial.printf("pomp on =%d\npomp off=%d\n ",config.PUMP_ON_DURATION,config.PUMP_OFF_DURATION);
Serial.printf("configversion = %s\n",config.ConfigVersion.c_str());
Serial.printf("password = %s\n",config.password.c_str());
Serial.printf("ssid = %s\n",config.ssid.c_str());
Serial.printf("SENSOR_CYCLE = %d\nLIGHT_THRESHOLD = %d\n",config.SENSOR_CYCLE,config.LIGHT_THRESHOLD);
}

void setup() 
{
  Serial.begin(115200);
  vTaskDelay(pdMS_TO_TICKS(1000));//to ensure 
  Serial.println("in setup");

  
 pinMode(PUMP_RELAY, OUTPUT);
 pinMode(SENSOR_RELAY, OUTPUT);
 pinMode(LIGHT_DATA, INPUT);
 pinMode(WATERLEVEL_DATA, INPUT);

 pinMode(LIGHT_RELAY, OUTPUT);
 pinMode(LED, OUTPUT);
 

 digitalWrite(PUMP_RELAY, HIGH);//make pump off
 digitalWrite(SENSOR_RELAY, HIGH);//make dht off
 digitalWrite(LIGHT_RELAY,HIGH);//make light off
 digitalWrite(LED, LOW);//MAKE LED OFF

 delay(100);

LoadConfig();

dht.begin();

WiFi.mode(WIFI_STA);
  
  SensorQueue=xQueueCreate(2, sizeof(SensorData));
  LightQueue=xQueueCreate(2,sizeof(float));
  HourQueue=xQueueCreate(2,sizeof(int));
  ResponseQueue=xQueueCreate(2,sizeof(ResponseData));

  PumpTimer = xTimerCreate("PumpTimer", pdMS_TO_TICKS(5000), pdTRUE, NULL, PumpCallback);
  SensorTimer = xTimerCreate("SensorTimer", pdMS_TO_TICKS(5000), pdTRUE, NULL, SensorCallback);

   if (PumpTimer != NULL && SensorTimer != NULL) 
   {
    Serial.println(" create timers succesfully!");
   }

   xTimerStart(PumpTimer, 0);
   xTimerStart(SensorTimer, 0);
   Serial.println("Timers started");

  xTaskCreate(WifiTask, "Wifi", 4096, NULL, 1, &WifiTaskHandle);
  xTaskCreate(HttpClient, "Http", 8192, NULL, 1, &HttpClientHandle);
  xTaskCreate(LightControlTask,"LightControl",4096,NULL,1,&LightControlHandle);
  xTaskCreate(Setting, "Settig", 8192, NULL, 1, &SettingHandle);
  

  
  
  
  Serial.println("setup complete");
}

void loop() 
{
  
}
