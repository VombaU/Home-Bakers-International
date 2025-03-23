using System.Collections.Generic;
using System.Threading.Tasks;
using Frontend.Models;

namespace Frontend.Services
{
    public interface IRecipeService
    {
        Task<Recipe> CreateRecipeAsync(Recipe recipe);
        Task<Recipe> GetRecipeByIdAsync(int id);
        Task<List<Recipe>> GetAllRecipesAsync();
        Task<Recipe> UpdateRecipeAsync(Recipe recipe);
        Task DeleteRecipeAsync(int id);

        Task<List<Recipe>> GetFilteredRecipesAsync(
            string cuisine = null,
            string doughType = null,
            string flourType = null,
            string kneadingMethod = null,
            string sortBy = null,
            bool ascending = true);
    }
}