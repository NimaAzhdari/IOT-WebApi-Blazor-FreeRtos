using System.ComponentModel.DataAnnotations;

public class MicroConfig
{

    [Key]
    public int id {get;set;}
    public string DeviceId { get; set; }
    public string ConfigVersion { get; set; }
    public int PUMP_ON_DURATION { get; set; }
    public int PUMP_OFF_DURATION { get; set; }
    public int SENSOR_CYCLE { get; set; }
    public string ssid { get; set; }
    public string password { get; set; }
    public int LIGHT_THRESHOLD { get; set; }
    public DateTime LastUpdate { get; set; }

}