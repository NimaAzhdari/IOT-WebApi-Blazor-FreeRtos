
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

[Route("api/[controller]")]
[ApiController]
public class Config (AppDbContext db):ControllerBase
{
[HttpPost("SetConfig")]
public async Task<IActionResult> SetConfig([FromBody] MicroConfigDto data)
{

MicroConfig config=new()
{
DeviceId="bb505cb3-ee8e-4c70-9b11-bdba7c210bb0",
ConfigVersion=Guid.NewGuid().ToString(),
PUMP_ON_DURATION=data.PUMP_ON_DURATION,
PUMP_OFF_DURATION=data.PUMP_OFF_DURATION,
SENSOR_CYCLE=data.SENSOR_CYCLE,
ssid=data.ssid,
password=data.password,
LIGHT_THRESHOLD=data.LIGHT_THRESHOLD,
LastUpdate=DateTime.Now
};
try
    {
    await db.MicroConfig.AddAsync(config);
    await db.SaveChangesAsync();
    return Ok();
    }
catch (Exception ex)
    {
    return BadRequest(ex);
    }
}
[HttpGet("GetConfig/{deviceid}")]
public async Task<IActionResult> GetConfig(string deviceid)
{
        try
        {
            var config = await
            (
                from row in db.MicroConfig
                where row.DeviceId == deviceid
                orderby row.LastUpdate descending
                select row
            ).FirstOrDefaultAsync();

            MicroConfigDto LastConfig = new()
            {
                ssid = config.ssid,
                password = config.password,
                PUMP_OFF_DURATION = config.PUMP_OFF_DURATION,
                PUMP_ON_DURATION = config.PUMP_ON_DURATION,
                SENSOR_CYCLE = config.SENSOR_CYCLE,
                LIGHT_THRESHOLD = config.LIGHT_THRESHOLD
            };

            return Ok(LastConfig);
    }
        catch (Exception ex)
        {
            return BadRequest(ex);
        }
}





}