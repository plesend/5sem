using Microsoft.AspNetCore.Mvc;
namespace lab01.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ResultController : Controller
    {

        public IActionResult Index()
        {
            return View();
        }
    }
}
