public class ChartDto
{
    public List<SensorData>? SensorData { get; set; }
    public float WaterLevel { get; set; }
    public bool LightIsOn { get; set; }


}
public class SensorData
{
    public float Light {get;set;} 
    public float Humidity {get;set;} 
    public float Temperature {get;set;} 
    public string Time {get;set;} 
}