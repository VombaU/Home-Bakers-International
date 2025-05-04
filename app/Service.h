#pragma once
#include "Recipes.h"
#include <ranges>
#include <algorithm>
#include <chrono>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace Backend {
	class Service {
	private:
		std::vector<Recipe> recipes;
		std::vector<Recipe> filteredRecipes;

		const std::unordered_set<std::string> KNEADING_METHODS{ "Cold Fermentation", "Preferment", "No-Knead" };
		const std::unordered_set<std::string> FLOUR_TYPES{ "All Purpose", "Whole Wheat", "Light Rye" };
		const std::unordered_set<std::string> CUISINES{ "Turkish", "Romanian", "English" };
		const std::unordered_set<std::string> DOUGH_TYPES{ "Sourdough", "Pastry", "Flatbread" };
		const std::unordered_map<std::string, std::function<bool(const Recipe&, const Recipe&)>> SORTING_CRITERIA{
			{"title", [](const Recipe& a, const Recipe& b) {
				for (int i = 0; i < std::min(a.Title.length(), b.Title.length()); ++i) {
					char ca = std::tolower(a.Title[i]);
					char cb = std::tolower(b.Title[i]);
					if (ca < cb) {
						return true;
					}
					else if (ca > cb) {
						return false;
					}
				}
				return false;
			}},
			{"date", [](const Recipe& a, const Recipe& b) { return a.UploadDate < b.UploadDate; } },
			{"rating", [](const Recipe& a, const Recipe& b) { return a.Rating < b.Rating; } }
		};

		void FillDefault();

	public:
		Service();
		Json::Value GetAllAsJson();
		void AddRecipe(Recipe recipe);
		bool DeleteRecipeById(int recipeId);
		bool UpdateRecipeById(int recipeId, Recipe newRecipe);

		Json::Value GetFilteredRecipesAsJson();
		void ResetFilteredRecipes();
		void FilterRecipesByKneadingMethod(std::string kneadingMethod);
		void FilterRecipesByFlourType(std::string flourType);
		void FilterRecipesByCuisine(std::string cuisine);
		void FilterRecipesByDoughType(std::string doughType);
		void SortFilteredRecipesBy(std::string sortingCriterion, bool ascending);
	};

	//struct KneadingMethods {
	//	inline static std::string ColdFermentation = "Cold Fermentation";
	//	inline static std::string Preferment = "Preferment";
	//	inline static std::string NoKnead = "No-Knead";
	//};

	//struct FlourTypes {
	//	inline static std::string AllPurpose = "All Purpose";
	//	inline static std::string WholeWheat = "Whole Wheat";
	//	inline static std::string LightRye = "Light Rye";
	//};

	//struct Cuisines {
	//	inline static std::string Turkish = "Turkish";
	//	inline static std::string Romanian = "Romanian";
	//	inline static std::string English = "English";
	//};

	//struct DoughTypes {
	//	inline static std::string Sourdough = "Sourdough";
	//	inline static std::string Pastry = "Pastry";
	//	inline static std::string Flatbread = "Flatbread";
	//};

	//struct SortingCriteria {
	//	inline static std::string Title = "title";
	//	inline static std::string Rating = "rating";
	//	inline static std::string Date = "date";
	//};
}