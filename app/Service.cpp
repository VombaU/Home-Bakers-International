#include "Service.h"

namespace Backend {
	void Service::FillDefault() {
		recipes.push_back(Recipe(
			2,	// id
			"Buttery Croissant Delight", // title
			"Crispy on the outside, soft and buttery on the inside, a perfect pastry treat.\n\nIngredients: 500g bread flour, 10g salt, 55g sugar, 10g instant yeast, 300g cold butter, 300ml cold milk.\n\nPreparation: Laminate dough with butter using letter folds, chill between folds. Shape into triangles, roll into crescents, proof for 2 hours, brush with egg wash, bake at 400°F for 15-20 minutes until golden.\n\nSource: Inspired by Julia Child's Mastering the Art of French Cooking with modern adjustments for home bakers.",
			"https://example.com/images/croissant.jpg", // thumbnail URL
			3.5, // rating
			"Romanian", // cuisine
			"Pastry", // dough type
			"Whole Wheat", // flour type
			"Preferment", // kneading method
			std::chrono::system_clock::now() // upload date
		));
		recipes.push_back(Recipe(
			1,
			"Rustic Sourdough Loaf",
			"A tangy, chewy sourdough loaf with a crispy crust and soft interior.\n\nIngredients: 500g bread flour, 350g water, 100g active sourdough starter, 10g salt.\n\nPreparation: Mix ingredients, perform stretch and folds every 30 minutes for 3 hours, cold ferment overnight, shape, proof for 2 hours, and bake at 450°F for 35 minutes.\n\nSource: Adapted from King Arthur Flour's classic sourdough method.",
			"https://example.com/images/sourdough.jpg",
			4.5,
			"English",
			"Sourdough",
			"All Purpose",
			"No-Knead",
			std::chrono::system_clock::now()
		));
		recipes.push_back(Recipe(
			3,
			"Traditional Turkish Flatbread",
			"A thin and soft flatbread, perfect for wraps or as an accompaniment to meals.\n\nIngredients: 500g all-purpose flour, 300ml warm water, 7g instant yeast, 10g salt, 30ml olive oil, 5g sugar.\n\nPreparation: Mix all ingredients, knead until smooth, divide into 8 pieces, cold ferment for 24 hours, roll thin, cook in a very hot cast iron pan or on a pizza stone at 500°F for 2 minutes per side.\n\nSource: Based on an authentic recipe from Istanbul, shared by Chef Mehmet Gurs of Mikla Restaurant.",
			"https://example.com/images/flatbread.jpg",
			5.0,
			"Turkish",
			"Flatbread",
			"Light Rye",
			"Cold Fermentation",
			std::chrono::system_clock::now()
		));
	}
	
	Service::Service() {
		FillDefault();
	}

	Json::Value Service::GetAllAsJson() {
		Json::Value root;
		for (auto& recipe : recipes) {
			root.append(recipe.ToJson());
		}
		return root;
	}

	void Service::AddRecipe(Recipe recipe) {
		recipes.push_back(recipe);
	}

	bool Service::DeleteRecipeById(int recipeId) {
		auto iterator = std::find_if(recipes.begin(), recipes.end(), [recipeId](Recipe recipe) { return recipe.Id == recipeId; });
		if (iterator != recipes.end()) {
			recipes.erase(iterator);
			return true;
		}
		return false;
	}

	bool Service::UpdateRecipeById(int recipeId, Recipe newRecipe) {
		auto iterator = std::find_if(recipes.begin(), recipes.end(), [recipeId](Recipe recipe) { return recipe.Id == recipeId; });
		if (iterator != recipes.end()) {
			*iterator = newRecipe;
			return true;
		}
		return false;
	}

	Json::Value Service::GetFilteredRecipesAsJson() {
		Json::Value root;
		for (auto& recipe : filteredRecipes) {
			root.append(recipe.ToJson());
		}
		return root;
	}

	void Service::ResetFilteredRecipes() {
		filteredRecipes = recipes;
	}

	void Service::FilterRecipesByKneadingMethod(std::string kneadingMethod) {
		if (KNEADING_METHODS.find(kneadingMethod) == KNEADING_METHODS.cend()) {
			return;
		}
		std::erase_if(filteredRecipes, [&kneadingMethod](const Recipe& recipe) { return recipe.KneadingMethod != kneadingMethod; });
	}

	void Service::FilterRecipesByFlourType(std::string flourType) {
		if (FLOUR_TYPES.find(flourType) == FLOUR_TYPES.cend()) {
			return;
		}
		std::erase_if(filteredRecipes, [&flourType](const Recipe& recipe) { return recipe.FlourType != flourType; });
	}

	void Service::FilterRecipesByCuisine(std::string cuisine) {
		if (CUISINES.find(cuisine) == CUISINES.cend()) {
			return;
		}
		std::erase_if(filteredRecipes, [&cuisine](const Recipe& recipe) { return recipe.Cuisine != cuisine; });
	}

	void Service::FilterRecipesByDoughType(std::string doughType) {
		if (DOUGH_TYPES.find(doughType) == DOUGH_TYPES.cend()) {
			return;
		}
		std::erase_if(filteredRecipes, [&doughType](const Recipe& recipe) { return recipe.DoughType != doughType; });
	}

	void Service::SortFilteredRecipesBy(std::string sortingCriterion, bool ascending) {
		try {
			std::ranges::sort(filteredRecipes, SORTING_CRITERIA.at(sortingCriterion));
			if (!ascending) {
				std::ranges::reverse(filteredRecipes);
			}
		}
		catch (std::out_of_range& err) {
			return;
		}
	}
}