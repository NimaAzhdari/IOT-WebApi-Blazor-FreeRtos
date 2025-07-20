
using Microsoft.EntityFrameworkCore;
public class AppDbContext :DbContext
{
public AppDbContext(DbContextOptions<AppDbContext> option):base(option)
 {}
 public DbSet<MicroData> MicroData{ get; set; }
 public DbSet<MicroConfig> MicroConfig{ get; set; }
}



