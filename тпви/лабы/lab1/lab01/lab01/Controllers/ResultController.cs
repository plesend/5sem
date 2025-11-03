using lab01.Models;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.IdentityModel.Tokens;
using ResultsAuthenticateLib;
using ResultsCollectionLib;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Text;

namespace ASPA01.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ResultsController : ControllerBase
    {
        private readonly ResultsCollectionLib.Results _resultsService;
        private readonly AuthenticateService _authService;

        public ResultsController(ResultsCollectionLib.Results resultsService, AuthenticateService authService)
        {
            _resultsService = resultsService;
            _authService = authService;
        }

        [HttpPost("SignIn")]
        [AllowAnonymous]
        public async Task<IActionResult> SignIn([FromBody] SignInModel profile)
        {
            if (string.IsNullOrEmpty(profile.Login) || string.IsNullOrEmpty(profile.Password))
            {
                return StatusCode(StatusCodes.Status400BadRequest, "Username or password is empty");
            }

            var res = await _authService.SignInAsync(profile.Login, profile.Password);
            if (!res.Succeeded)
            {
                return StatusCode(StatusCodes.Status401Unauthorized, "Invalid username or password");
            }

            return Ok(res);
        }

        [HttpGet("SignOut")]
        [Authorize]
        public async Task<IActionResult> SignOut()
        {
            await _authService.SignOut();
            return Ok(new { message = "Signed out successfully" });
        }

        [HttpGet]
        [Authorize(Roles = "READER")]
        public IActionResult GetAllResults()
        {
            if (!User.Identity?.IsAuthenticated ?? false)
            {
                return StatusCode(StatusCodes.Status401Unauthorized, "User is not authorized");
            }

            var results = _resultsService.GetAllResults();
            if (results.Count == 0)
                return NotFound("No results found");

            return Ok(results);
        }

        [HttpGet("{id:int}")]
        [Authorize(Roles = "READER")]
        public IActionResult GetById(int id)
        {
            if (!User.Identity?.IsAuthenticated ?? false)
            {
                return StatusCode(StatusCodes.Status401Unauthorized, "User is not authorized");
            }

            var result = _resultsService.GetResultById(id);
            return result == null
                ? NotFound($"Result with ID {id} not found")
                : Ok(result);
        }

        [HttpPost]
        [Authorize(Roles = "WRITER")]
        public IActionResult Add([FromBody] string value)
        {
            if (!User.Identity?.IsAuthenticated ?? false)
            {
                return StatusCode(StatusCodes.Status401Unauthorized, "User is not authorized");
            }

            if (string.IsNullOrEmpty(value))
                return BadRequest("Value cannot be empty");

            _resultsService.AddResult(value);
            return Ok(new { message = "Result added successfully" });
        }

        [HttpPut("{id:int}")]
        [Authorize(Roles = "WRITER")]
        public IActionResult Update(int id, [FromBody] string value)
        {
            if (!User.Identity?.IsAuthenticated ?? false)
            {
                return StatusCode(StatusCodes.Status401Unauthorized, "User is not authorized");
            }

            if (string.IsNullOrEmpty(value))
                return BadRequest("Value cannot be empty");

            var existing = _resultsService.GetResultById(id);
            if (existing == null)
                return NotFound($"Result with ID {id} not found");

            _resultsService.ChangeResultById(id, value);
            return Ok(new { message = "Result updated successfully" });
        }

        [HttpDelete("{id:int}")]
        [Authorize(Roles = "WRITER")]
        public IActionResult Delete(int id)
        {
            if (!User.Identity?.IsAuthenticated ?? false)
            {
                return StatusCode(StatusCodes.Status401Unauthorized, "User is not authorized");
            }

            var existing = _resultsService.GetResultById(id);
            if (existing == null)
                return NotFound($"Result with ID {id} not found");

            _resultsService.DeleteResult(id);
            return Ok(new { message = "Result deleted successfully" });
        }

        private string GenerateJWT()
        {
            var key = new SymmetricSecurityKey(Encoding.UTF8.GetBytes("16-chars-minimum-key-secret-super"));
            var credentials = new SigningCredentials(key, SecurityAlgorithms.HmacSha256);

            var token = new JwtSecurityToken(
                issuer: "bstu",
                audience: "bstu",
                claims: new Claim[] { },
                expires: DateTime.UtcNow.AddSeconds(100),
                signingCredentials: credentials);

            return new JwtSecurityTokenHandler().WriteToken(token);
        }
    }
}
