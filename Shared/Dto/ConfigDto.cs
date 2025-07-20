public class ConfigDto//to send config to micro
{
    public bool ischanged { get; set; }
    public string configversion { get; set; }
    public int pump_on_duration { get; set; }
    public int pump_off_duration { get; set; }
    public int sensor_cycle { get; set; }
    public string ssid { get; set; }
    public string password { get; set; }
    public int light_threshold { get; set; }
    
}