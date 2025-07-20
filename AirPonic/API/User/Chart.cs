
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

[Route("api/[controller]")]
[ApiController]
public class Chart(AppDbContext db) :ControllerBase
{

[HttpGet("light")]
public async Task<IActionResult> LightData()
{
    var today = DateTime.Today;
    var tommorow = today.AddDays(1);

    var sensordata=await (from rows in db.MicroData
                  where rows.Timestamp >=today && rows.Timestamp <= tommorow
                  orderby rows.Timestamp 
                  select rows).ToListAsync();
                  
    

    var response_first = (from data in sensordata
                    select new SensorData{
                        Light =data.Light  ,
                        Humidity = data.Humidity,
                        Temperature = data.Temperature,
                        Time =data.Timestamp.ToString("HH:mm")
                    }).ToList();

        var response_second = (from data in sensordata
                               orderby data.Timestamp descending
                               select new
                               {
                                   WaterLevel = data.WaterLevel,
                                   LightIsOn=data.LightIsOn
                               }
                             ).FirstOrDefault();

        ChartDto response = new()
        {
            SensorData = response_first,

            WaterLevel = response_second?.WaterLevel ?? 0,
            
            LightIsOn = response_second?.LightIsOn ?? false
            
        };

    return Ok(response);
}


}