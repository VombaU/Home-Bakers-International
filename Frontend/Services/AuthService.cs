using Blazored.LocalStorage;
using System;
using System.Net.Http.Json;
using System.Text.Json;


namespace Frontend.Services
{
    public class AuthService
    {
        public bool IsAuthenticated { get; private set; } = false; // Default to false
        public string Username { get; private set; }
        private HttpClient m_httpClient;
        private readonly ILocalStorageService m_localStorage;

        public event Action OnAuthStateChanged;

        public AuthService(HttpClient httpClient, ILocalStorageService localStorage) {
            m_httpClient = httpClient;
            m_localStorage = localStorage;
        }

        public async Task Login(string username, string email, string password)
        {
            HttpResponseMessage res = await m_httpClient.PostAsJsonAsync("/login", new { username = username, email = email, password = password });
            if (!res.IsSuccessStatusCode)
            {
                throw new ArgumentException($"Error {res.StatusCode}: {await res.Content.ReadAsStringAsync()}");
            }
            await m_localStorage.SetItemAsync("username", username);
        }

        public async Task Register(string username, string email, string password)
        {
            HttpResponseMessage res = await m_httpClient.PostAsJsonAsync("/register", new { username = username, email = email, password = password });
            if (!res.IsSuccessStatusCode)
            {
                throw new ArgumentException($"Error {res.StatusCode}: {await res.Content.ReadAsStringAsync()}");
            }
            await m_localStorage.SetItemAsync("username", username);
        }

        public async Task ConfirmOtp(string otp)
        {
            HttpResponseMessage res = await m_httpClient.PostAsJsonAsync("/verifyOTP", new { username = (await m_localStorage.GetItemAsync<string>("username")), otp = otp });
            if (!res.IsSuccessStatusCode)
            {
                throw new ArgumentException($"Error {res.StatusCode}: {await res.Content.ReadAsStringAsync()}");
            }
            var resJson = await res.Content.ReadAsStringAsync();
            var token = JsonDocument.Parse(resJson).RootElement.GetProperty("accessToken").ToString();
            await m_localStorage.SetItemAsync("accessToken", token);
            IsAuthenticated = true;
            NotifyAuthStateChanged();
        }

        public void Logout()
        {
            Username = "";
            IsAuthenticated = false;
            NotifyAuthStateChanged();
        }

        private void NotifyAuthStateChanged() => OnAuthStateChanged?.Invoke();
    }
} 