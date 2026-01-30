using ASPA0011.Models;
using Microsoft.OpenApi.Models;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Orleans.TestingHost.Logging;

var builder = WebApplication.CreateBuilder(args);

// Logging

builder.Logging.ClearProviders()
    .AddConsole()
    .AddDebug()
    
    .AddFile("./Logs/app.log");

builder.Logging.AddFilter("Microsoft", LogLevel.Trace);
builder.Logging.AddFilter("System", LogLevel.Trace);
builder.Logging.AddFilter("ASPA0011", LogLevel.Trace);


if (builder.Environment.IsDevelopment())
{
    builder.Logging.SetMinimumLevel(LogLevel.Trace);
}

// Services
builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen(c => 
    c.SwaggerDoc("v1", new OpenApiInfo { Title = "Channels API", Version = "v1" }));

builder.Services.AddSingleton<ChannelService>();

var app = builder.Build();

if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI(c => c.SwaggerEndpoint("/swagger/v1/swagger.json", "v1"));
}

app.UseHttpsRedirection();
app.UseAuthorization();
app.MapControllers();

app.Run();