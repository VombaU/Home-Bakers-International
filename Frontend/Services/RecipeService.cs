using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Frontend.Models;

namespace Frontend.Services
{
    public class RecipeService : IRecipeService
    {
        private readonly List<Recipe> _recipes = new();
        private int _nextId = 4; // Default recipes have IDs 1-3

        public RecipeService()
        {
            _recipes.Add(new Recipe {
                Id = 1,
                Title = "Rustic Sourdough Loaf",
                Description = "A tangy, chewy sourdough loaf with a crispy crust and soft interior.\n\nIngredients: 500g bread flour, 350g water, 100g active sourdough starter, 10g salt.\n\nPreparation: Mix ingredients, perform stretch and folds every 30 minutes for 3 hours, cold ferment overnight, shape, proof for 2 hours, and bake at 450°F for 35 minutes.\n\nSource: Adapted from King Arthur Flour's classic sourdough method.",
                ThumbnailUrl = "https://example.com/images/sourdough.jpg",
                Rating = 4.5,
                UploadDate = DateTime.Now,
                Cuisine = "English",
                DoughType = "Sourdough",
                FlourType = "All Purpose",
                KneadingMethod = "No-Knead"
            });

            _recipes.Add(new Recipe {
                Id = 2,
                Title = "Buttery Croissant Delight",
                Description = "Crispy on the outside, soft and buttery on the inside, a perfect pastry treat.\n\nIngredients: 500g bread flour, 10g salt, 55g sugar, 10g instant yeast, 300g cold butter, 300ml cold milk.\n\nPreparation: Laminate dough with butter using letter folds, chill between folds. Shape into triangles, roll into crescents, proof for 2 hours, brush with egg wash, bake at 400°F for 15-20 minutes until golden.\n\nSource: Inspired by Julia Child's Mastering the Art of French Cooking with modern adjustments for home bakers.",
                ThumbnailUrl = "https://example.com/images/croissant.jpg",
                Rating = 3.5,
                UploadDate = DateTime.Now,
                Cuisine = "Romanian",
                DoughType = "Pastry",
                FlourType = "Whole Wheat",
                KneadingMethod = "Preferment"
            });

            _recipes.Add(new Recipe {
                Id = 3,
                Title = "Traditional Turkish Flatbread",
                Description = "A thin and soft flatbread, perfect for wraps or as an accompaniment to meals.\n\nIngredients: 500g all-purpose flour, 300ml warm water, 7g instant yeast, 10g salt, 30ml olive oil, 5g sugar.\n\nPreparation: Mix all ingredients, knead until smooth, divide into 8 pieces, cold ferment for 24 hours, roll thin, cook in a very hot cast iron pan or on a pizza stone at 500°F for 2 minutes per side.\n\nSource: Based on an authentic recipe from Istanbul, shared by Chef Mehmet Gurs of Mikla Restaurant.",
                ThumbnailUrl = "https://example.com/images/flatbread.jpg",
                Rating = 5.0,
                UploadDate = DateTime.Now,
                Cuisine = "Turkish",
                DoughType = "Flatbread",
                FlourType = "Light Rye",
                KneadingMethod = "Cold Fermentation"
            });
        }

        public Task<List<Recipe>> GetAllRecipesAsync()
        {
            return Task.FromResult(_recipes);
        }

        public Task<Recipe> GetRecipeByIdAsync(int id)
        {
            return Task.FromResult(_recipes.FirstOrDefault(r => r.Id == id));
        }

        public Task<Recipe> CreateRecipeAsync(Recipe recipe)
        {
            recipe.Id = _nextId++;
            recipe.UploadDate = DateTime.Now;
            _recipes.Add(recipe);
            return Task.FromResult(recipe);
        }

        public Task<Recipe> UpdateRecipeAsync(Recipe recipe)
        {
            var existingRecipe = _recipes.FirstOrDefault(r => r.Id == recipe.Id);
            if (existingRecipe != null)
            {
                recipe.ModifiedDate = DateTime.Now;
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
                _recipes.Remove(recipe);
            }
            return Task.CompletedTask;
        }

        public Task<List<Recipe>> GetFilteredRecipesAsync(
            string cuisine = null,
            string doughType = null,
            string flourType = null,
            string kneadingMethod = null,
            string sortBy = null,
            bool ascending = true)
        {
            var query = _recipes.AsQueryable();

            if (!string.IsNullOrEmpty(cuisine))
                query = query.Where(r => r.Cuisine == cuisine);
            if (!string.IsNullOrEmpty(doughType))
                query = query.Where(r => r.DoughType == doughType);
            if (!string.IsNullOrEmpty(flourType))
                query = query.Where(r => r.FlourType == flourType);
            if (!string.IsNullOrEmpty(kneadingMethod))
                query = query.Where(r => r.KneadingMethod == kneadingMethod);

            if (!string.IsNullOrEmpty(sortBy))
            {
                query = sortBy.ToLower() switch
                {
                    "title" => ascending ? query.OrderBy(r => r.Title) : query.OrderByDescending(r => r.Title),
                    "rating" => ascending ? query.OrderBy(r => r.Rating) : query.OrderByDescending(r => r.Rating),
                    "date" => ascending ? query.OrderBy(r => r.UploadDate) : query.OrderByDescending(r => r.UploadDate),
                    _ => query
                };
            }

            return Task.FromResult(query.ToList());
        }
    }
} 