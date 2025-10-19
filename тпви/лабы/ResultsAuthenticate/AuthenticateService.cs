namespace ResultsAuthenticateLib
{
    public class AuthenticateService
    {
        private readonly Dictionary<string, (string Password, string Role)> _users;

        public AuthenticateService()
        {
            _users = new Dictionary<string, (string, string)>
            {
                { "reader1", ("123", "READER") },
                { "writer1", ("123", "WRITER") }
            };
        }

        public (bool Success, string Role) ValidateUser(string login, string password)
        {
            if (_users.TryGetValue(login, out var user))
            {
                if (user.Password == password)
                    return (true, user.Role);
            }
            return (false, null);
        }
    }

    public class SignInModel
    {
        public string Login { get; set; }
        public string Password { get; set; }
    }
}
