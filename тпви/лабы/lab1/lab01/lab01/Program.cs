using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using ResultsAuthenticateLib;
using ResultsCollectionLib;
using lab01;
var builder = WebApplication.CreateBuilder(args);

builder.Services.AddDbContext<dbcontext>(options =>
    options.UseSqlite("Data Source=lab01.db",
     b => b.MigrationsAssembly("lab01")));

builder.Services.AddSingleton(sp =>
{
    var path = @"D:\лабораторные работы\тпви\лабы\lab1\lab01\tpvi.json";
    return new ResultsCollectionLib.Results(path);
});

builder.Services.AddIdentity<IdentityUser, IdentityRole>(options=>
{
    options.Password.RequireLowercase = false;
    options.Password.RequireUppercase = false;
    options.Password.RequireNonAlphanumeric = false;
    options.Password.RequiredLength = 6;
})
    .AddEntityFrameworkStores<dbcontext>()
    .AddDefaultTokenProviders();


builder.Services.AddScoped<AuthenticateService>();


builder.Services.AddControllers();

// Swagger/OpenAPI
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen(c =>
{

    c.AddSecurityRequirement(new Microsoft.OpenApi.Models.OpenApiSecurityRequirement
    {
        {
            new Microsoft.OpenApi.Models.OpenApiSecurityScheme
            {
                Reference = new Microsoft.OpenApi.Models.OpenApiReference
                {
                    Type = Microsoft.OpenApi.Models.ReferenceType.SecurityScheme,
                    Id = "cookieAuth"
                }
            },
            new string[] {}
        }
    });
});


builder.Services.AddAuthorization();

var app = builder.Build();


using (var scope = app.Services.CreateScope())
{
    
    var roleManager = scope.ServiceProvider.GetRequiredService<RoleManager<IdentityRole>>();
    var userManager = scope.ServiceProvider.GetRequiredService<UserManager<IdentityUser>>();

    if (!await roleManager.RoleExistsAsync("READER"))
        await roleManager.CreateAsync(new IdentityRole("READER"));


    if (!await roleManager.RoleExistsAsync("WRITER"))
        await roleManager.CreateAsync(new IdentityRole("WRITER"));


    var writerUser = await userManager.FindByNameAsync("writer");
    if (writerUser == null)
    {
        writerUser = new IdentityUser("writer"); 
        await userManager.CreateAsync(writerUser, "123456");
        await userManager.AddToRoleAsync(writerUser, "WRITER");
    }
    var readerUser = await userManager.FindByNameAsync("reader");
    if (readerUser == null)
    {
        readerUser = new IdentityUser { UserName = "reader" };
        await userManager.CreateAsync(readerUser, "123456"); 
        await userManager.AddToRoleAsync(readerUser, "READER");
    }
}

// Swagger
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}
else
{
    app.UseExceptionHandler("/Home/Error");
    app.UseHsts();
}

app.UseHttpsRedirection();
app.UseStaticFiles();

app.UseRouting();

app.UseAuthentication();
app.UseAuthorization();

app.MapControllers();

app.Run();