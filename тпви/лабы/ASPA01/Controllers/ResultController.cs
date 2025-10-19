using ASPA01.Models;
using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using ResultsAuthenticateLib;
using ResultsCollectionLib;
using System.Security.Claims;

namespace ASPA01.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ResultController : Controller
    {
        private readonly ResultsCollectionLib.Results _resultsService;
        private readonly AuthenticateService _authService;

        public ResultController(ResultsCollectionLib.Results resultsService, AuthenticateService authService)
        {
            _resultsService = resultsService;
            _authService = authService;
        }

        [HttpGet]
        [Authorize(Roles = "READER,WRITER")]
        public IActionResult GetAll()
        {
            var results = _resultsService.GetAllResults();
            if (results == null || results.Count == 0)
                return NoContent(); // 204

            return Ok(results); // 200
        }

        [HttpGet("{id:int}")]
        [Authorize(Roles = "READER,WRITER")]
        public IActionResult GetById(int id)
        {
            var result = _resultsService.GetResultById(id);
            if (result == null)
                return NotFound(); // 404

            return Ok(result); // 200
        }

        [HttpPost]
        [Authorize(Roles = "WRITER")]
        public IActionResult Add([FromBody] ResultDto result)
        {
            if (result == null || string.IsNullOrWhiteSpace(result.Value))
                return BadRequest(); // 400

            _resultsService.AddResult(result.Value);
            return StatusCode(StatusCodes.Status201Created); // 201
        }

        [HttpPut("{id:int}")]
        [Authorize(Roles = "WRITER")]
        public IActionResult Update(int id, [FromBody] Result result)
        {
            if (result == null || string.IsNullOrWhiteSpace(result.Value))
                return BadRequest(); // 400

            var existing = _resultsService.GetResultById(id);
            if (existing == null)
                return NotFound(); // 404

            _resultsService.ChangeResultById(id, result.Value);
            return Ok(); // 200
        }

        [HttpDelete("{id:int}")]
        [Authorize(Roles = "WRITER")]
        public IActionResult Delete(int id)
        {
            var existing = _resultsService.GetResultById(id);
            if (existing == null)
                return NotFound(); // 404

            _resultsService.DeleteResult(id);
            return Ok(); // 200
        }

        [HttpPost("SignIn")]
        public async Task<IActionResult> SignIn([FromBody] SignInModel model)
        {
            if (model == null || string.IsNullOrEmpty(model.Login) || string.IsNullOrEmpty(model.Password))
                return BadRequest();

            var (success, role) = _authService.ValidateUser(model.Login, model.Password);
            if (!success) return NotFound();

            var claims = new List<Claim>
        {
            new Claim(ClaimTypes.Name, model.Login),
            new Claim(ClaimTypes.Role, role)
        };
            var identity = new ClaimsIdentity(claims, "CookieAuth");
            var principal = new ClaimsPrincipal(identity);

            await HttpContext.SignInAsync("CookieAuth", principal);

            return Ok(new { Message = "Signed in" });
        }

        [HttpGet("SignOut")]
        public async Task<IActionResult> SignOut()
        {
            await HttpContext.SignOutAsync("CookieAuth");
            return Ok(new { Message = "Signed out" });
        }
    }
}
