using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http.Json;
using System.Text.Json;
using System.Threading.Tasks;
using Frontend.Models;

namespace Frontend.Services
{
    public class RecipeService : IRecipeService
    {
        private List<Recipe> _recipes = new();
        private readonly HttpClient _httpClient;

        public RecipeService(HttpClient httpClient)
        {
            _httpClient = httpClient;
        }

        public async Task<List<Recipe>> GetAllRecipesAsync()
        {
            try
            {
                Console.WriteLine("Fetching recipes from backend...");
                _recipes = await _httpClient.GetFromJsonAsync<List<Recipe>>("recipes");
                Console.WriteLine("Recipes fetched successfully");
                return _recipes;
            }
            catch (HttpRequestException httpEx)
            {
                Console.WriteLine($"HTTP Error: {httpEx.Message}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected Error: {ex.Message}");
            }
            return new List<Recipe>();
        }

        public Task<Recipe> GetRecipeByIdAsync(int id)
        {
            return Task.FromResult(_recipes.FirstOrDefault(r => r.Id == id));
        }

        public Task<Recipe> CreateRecipeAsync(Recipe recipe)
        {
            recipe.UploadDate = DateTime.UtcNow;
            _recipes.Add(recipe);
            _httpClient.PostAsJsonAsync("recipe/new", recipe);
            return Task.FromResult(recipe);
        }

        public Task<Recipe> UpdateRecipeAsync(Recipe recipe)
        {
            var existingRecipe = _recipes.FirstOrDefault(r => r.Id == recipe.Id);
            if (existingRecipe != null)
            {
                recipe.ModifiedDate = DateTime.UtcNow;
                _httpClient.PatchAsJsonAsync($"recipe/edit/{recipe.Id}", recipe);
                var index = _recipes.IndexOf(existingRecipe);
                _recipes[index] = recipe;
            }
            return Task.FromResult(recipe);
        }

        public Task DeleteRecipeAsync(int id)
        {
            var recipe = _recipes.FirstOrDefault(r => r.Id == id);
            if (recipe != null)
            {
                _httpClient.DeleteAsync($"recipes?id={id}");
                _recipes.Remove(recipe);
            }
            return Task.CompletedTask;
        }

        public async Task<List<Recipe>> GetFilteredRecipesAsync(
            string cuisine = null,
            string doughType = null,
            string flourType = null,
            string kneadingMethod = null,
            string sortBy = null,
            bool ascending = true)
        {
            string queryParams = "";
            queryParams += string.IsNullOrEmpty(cuisine) ? "cuisine=&" : $"cuisine={cuisine}&";
            queryParams += string.IsNullOrEmpty(doughType) ? "doughType=&" : $"doughType={doughType}&";
            queryParams += string.IsNullOrEmpty(flourType) ? "flourType=&" : $"flourType={flourType}&";
            queryParams += string.IsNullOrEmpty(kneadingMethod) ? "kneadingMethod=&" : $"kneadingMethod={kneadingMethod}&";
            queryParams += string.IsNullOrEmpty(sortBy) ? "sortBy=&ascending=" : $"sortBy={sortBy}&ascending={ascending}";

            try
            {
                Console.WriteLine("Fetching recipes from backend...");
                _recipes = await _httpClient.GetFromJsonAsync<List<Recipe>>("recipes?" + queryParams) ?? new List<Recipe>();
                Console.WriteLine("Recipes fetched successfully");
            }
            catch (HttpRequestException httpEx)
            {
                Console.WriteLine($"HTTP Error: {httpEx.Message}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected Error: {ex.Message}");
            }
            return _recipes;
        }
    }
} 