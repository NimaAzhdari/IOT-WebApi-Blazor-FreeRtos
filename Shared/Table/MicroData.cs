using System.ComponentModel.DataAnnotations;


public class MicroData
{
    [Key]
    public int id {get;set;}
    public string DeviceId { get; set; }
    public float Humidity { get; set; }
    public float Temperature{ get; set; }
    public float Light{ get; set; }
    public float WaterLevel{ get; set; }
    public bool LightIsOn { get; set; }
    public DateTime Timestamp { get; set; }
}
