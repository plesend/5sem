using Microsoft.AspNetCore.Identity;

namespace ResultsAuthenticateLib
{
    public class AuthenticateService
    {
        private UserManager<IdentityUser> _userManager;
        private SignInManager<IdentityUser> _signInManager;

        public AuthenticateService(UserManager<IdentityUser> userManager, SignInManager<IdentityUser> signInManager)
        {
            _userManager = userManager;
            _signInManager = signInManager;
        }
        public async Task<SignInResult> SignInAsync(string username, string password)
        {
            var user = await _userManager.FindByNameAsync(username);
            if (user != null)
            {
                return await _signInManager.PasswordSignInAsync(user, password, false, false);
            }
            return SignInResult.Failed;
        }
        public async Task SignOut()
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
