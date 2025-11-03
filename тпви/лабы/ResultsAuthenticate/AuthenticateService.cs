using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Identity.;

namespace ResultsAuthenticateLib
{
    public class AuthenticateService
    {
        private readonly SignInManager<IdentityUser> _signInManager;

        private readonly UserManager<IdentityUser> _userManager;

        public AuthenticateService(SignInManager<IdentityUser> signInManager, UserManager<IdentityUser> userManager)
        {
            _signInManager = signInManager;
            _userManager = userManager;
        }

        public async Task<(bool success, string message)> SignInAsync(SignInModel model)
        {
            var user = await _userManager.FindByNameAsync(model.Login);
            if (user == null)
                return (false, "User not found");

            var result = await _signInManager.PasswordSignInAsync(user, model.Password, false, false);
            if (!result.Succeeded)
                return (false, "Invalid password");

            return (true, "Signed in");
        }

        public async Task SignOutAsync()
        {
            await _signInManager.SignOutAsync();
        }

    }

    public class SignInModel
    {
        public string Login { get; set; }
        public string Password { get; set; }
    }
}
