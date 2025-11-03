using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;

namespace lab01
{
    public class dbcontext:IdentityDbContext<IdentityUser>
    {
        public dbcontext(DbContextOptions<dbcontext> options) : base(options) { }
    }
}
