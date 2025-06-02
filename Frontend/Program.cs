using Microsoft.AspNetCore.Components.Web;
using Microsoft.AspNetCore.Components.WebAssembly.Hosting;
using Frontend;
using Frontend.Services;
using Microsoft.AspNetCore.Components.WebAssembly.Http;
using Blazored.LocalStorage;
using System.Net.Http.Headers;


var builder = WebAssemblyHostBuilder.CreateDefault(args);
builder.RootComponents.Add<App>("#app");
builder.RootComponents.Add<HeadOutlet>("head::after");

builder.Services.AddScoped<CookieHandler>();
builder.Services.AddScoped(serviceProvider => new HttpClient(serviceProvider.GetService<CookieHandler>()) { BaseAddress = new Uri("http://localhost:8080/") });
builder.Services.AddBlazoredLocalStorage();

builder.Services.AddScoped<RecipeService>();
builder.Services.AddScoped<AuthService>();

await builder.Build().RunAsync();


public class CookieHandler : DelegatingHandler
{
    private readonly ILocalStorageService m_localStorage;
    public CookieHandler(ILocalStorageService localStorage)
    {
        InnerHandler = new HttpClientHandler();
        m_localStorage = localStorage;
    }

    protected async override Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken)
    {
        var token = await m_localStorage.GetItemAsync<string>("accessToken");
        if (!string.IsNullOrWhiteSpace(token))
        {
            request.Headers.Authorization =
                new AuthenticationHeaderValue("Bearer", token);
        }
        request.SetBrowserRequestCredentials(BrowserRequestCredentials.Include);
        return await base.SendAsync(request, cancellationToken);
    }
}
