using AirPonic.Client.Pages;
using AirPonic.Components;
using Microsoft.EntityFrameworkCore;
using Microsoft.VisualBasic;
using Radzen;
using Radzen.Blazor;
using Serilog;


var builder = WebApplication.CreateBuilder(args);


Log.Logger = new LoggerConfiguration()
    .WriteTo.File("Logs/log.txt", rollingInterval: RollingInterval.Day) 
    .CreateLogger();

builder.Logging.AddSerilog();

builder.Services.AddScoped<ThemeService>();
builder.Services.AddScoped<DialogService>();
builder.Services.AddScoped<NotificationService>();
builder.Services.AddScoped<TooltipService>();
builder.Services.AddScoped<ContextMenuService>();



builder.Services.AddHttpClient();

builder.Services.AddDbContext<AppDbContext>(option =>
{
    var connection = builder.Configuration.GetConnectionString("LocallConnection");
    option.UseMySql("Server=localhost;Database=fogponic_main;User=root;Password=waWfsc@14@&J;", ServerVersion.AutoDetect("Server=localhost;Database=fogponic_main;User=root;Password=waWfsc@14@&J;"));
}
);

builder.Services.AddControllers();


builder.Services.AddRazorComponents()
    .AddInteractiveWebAssemblyComponents();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseDeveloperExceptionPage();
    app.UseWebAssemblyDebugging();
}
else
{
    app.UseExceptionHandler("/Error", createScopeForErrors: true);
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}

app.UseHttpsRedirection();

app.UseStaticFiles();
app.UseAntiforgery();

app.UseCors(builder => builder
    .AllowAnyOrigin()
    .AllowAnyMethod()
    .AllowAnyHeader());

app.MapControllers();

app.MapRazorComponents<App>()
    .AddInteractiveWebAssemblyRenderMode()
    .AddAdditionalAssemblies(typeof(AirPonic.Client._Imports).Assembly);

app.Run();
