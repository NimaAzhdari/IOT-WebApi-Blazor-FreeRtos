using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using TimeZoneConverter;
[Route("api/[controller]")]
[ApiController]
public class Micro(AppDbContext db,ILogger<Micro> logger) : ControllerBase
{
    [HttpPost("SendData")]
    public async Task<IActionResult> SaveData([FromBody] MicroDataDto data)
    {
         var tehranTimeZone = TZConvert.GetTimeZoneInfo("Asia/Tehran");
         var tehranTime = TimeZoneInfo.ConvertTimeFromUtc(DateTime.UtcNow, tehranTimeZone);
         MicroData tabledata=new ()
        {
            DeviceId=data.DeviceId,
            Humidity=data.Humidity,
            Temperature=data.Temperature,
            Light=data.Light,
            LightIsOn =data.LightIsOn,
            WaterLevel=data.WaterLevel,
            Timestamp =tehranTime
        };
        try
        {
            await db.MicroData.AddAsync(tabledata);
            await db.SaveChangesAsync();
            var hour=tehranTime.Hour;
            return Ok(new{hour});
        }
        catch (Exception ex)
        {
            return BadRequest(ex);
        }
    }
    
   
    [HttpPost("GetConfig")]
    public async Task<IActionResult> GetConfig ([FromBody] ConfigData data)
    {

    bool ischanged = false;
    logger.LogInformation("in get config-> id={id} and version ={version} ",data.DeviceId,data.ConfigVersion );
    MicroConfig? config;
    try
    {
     config=await 
     (from row in db.MicroConfig
     where row.DeviceId == data.DeviceId 
     orderby row.LastUpdate descending
     select row).FirstOrDefaultAsync();
     logger.LogInformation("in get config -> db output is {config}",config);
    }
    catch(Exception ex)
    {
    return BadRequest(ex);
    }
    if (config == null)
    {
        return NotFound("Config not found for device.");
    }
    else
    {
      
        if(data.ConfigVersion == config.ConfigVersion)
        {   
            return Ok(new{ischanged});
        }
         else
         {
           ConfigDto response=new()
           {
            ischanged = true,
            configversion = config.ConfigVersion,
            pump_on_duration =config.PUMP_ON_DURATION,
            pump_off_duration =config.PUMP_OFF_DURATION,
            sensor_cycle =config.SENSOR_CYCLE,
            ssid =config.ssid,
            password =config.password,
            light_threshold =config.LIGHT_THRESHOLD
           };
            return Ok(response);
         }
    }
    

    }

    
}
public class ConfigData
{
    public string DeviceId{get;set;}
    public string ConfigVersion{get;set;}

}





