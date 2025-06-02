class Service {
  constructor(RecipeModel, UserModel) {
    this.RecipeModel = RecipeModel;
    this.UserModel = UserModel;
    this.recipes = [];
    this.filteredRecipes = [];

    this.KNEADING_METHODS = new Set(['Cold Fermentation', 'Preferment', 'No-Knead']);
    this.FLOUR_TYPES = new Set(['All Purpose', 'Whole Wheat', 'Light Rye']);
    this.CUISINES = new Set(['Turkish', 'Romanian', 'English']);
    this.DOUGH_TYPES = new Set(['Sourdough', 'Pastry', 'Flatbread']);

    this.SORTING_CRITERIA = new Map([
      ['title', (a, b) => {
        const sa = a.title.toLowerCase();
        const sb = b.title.toLowerCase();
        return sa < sb ? -1 : sa > sb ? 1 : 0;
      }],
      ['date', (a, b) => new Date(a.uploadDate) - new Date(b.uploadDate)],
      ['rating', (a, b) => a.rating - b.rating]
    ]);
  }

  async loadRecipes() {
    try {
      const dbRecipes = await this.RecipeModel.findAll();
      this.recipes = dbRecipes;
      this.resetFilteredRecipes();
    } catch (error) {
      console.error("Error loading recipes:", error);
      this.recipes = [];
      this.filteredRecipes = [];
      throw error;
    }
  }

  getAll() {
    return this.recipes;
  }

  async addRecipe(recipeData, username) {
    if (!username) {
      throw new Error("User must be logged in to add a recipe.");
    }
    try {
      const recipeToCreate = recipeData;
      recipeToCreate.userUsername = username;
      delete recipeToCreate.id;

      const newRecipe = await this.RecipeModel.create(recipeToCreate);
      this.recipes.push(newRecipe);
      this.resetFilteredRecipes();
      return newRecipe;
    } catch (error) {
      console.error("Error adding recipe:", error);
    }
  }

  async deleteRecipeById(recipeId, username) {
    if (!username) {
      throw new Error("User must be logged in to delete a recipe.");
    }
    try {
      const result = await this.RecipeModel.destroy({
        where: {
          id: recipeId,
          userUsername: username
        }
      });
      if (result > 0) {
        this.recipes = this.recipes.filter(r => r.id !== recipeId);
        return true;
      }
      return false;
    } catch (error) {
      console.error("Error deleting recipe:", error);
    }
  }

  async updateRecipeById(recipeId, newRecipeData, username) {
    if (!username) {
      throw new Error("User must be logged in to update a recipe.");
    }
    try {
      const recipeToUpdate = newRecipeData;
      delete recipeToUpdate.id;

      const [numberOfAffectedRows] = await this.RecipeModel.update(recipeToUpdate, {
        where: {
          id: recipeId,
          userUsername: username
        },
      });

      if (numberOfAffectedRows > 0) {
        const index = this.recipes.findIndex(r => r.id === recipeId && r.userUsername === username);
        if (index !== -1) {
          this.recipes[index] = newRecipeData;
          this.resetFilteredRecipes();
        }
        return true;
      }
      return false;
    } catch (error) {
      console.error("Error updating recipe:", error);
    }
  }

  getFiltered() {
    return this.filteredRecipes;
  }

  resetFilteredRecipes() {
    this.filteredRecipes = [...this.recipes];
  }

  filterByKneadingMethod(method) {
    if (!this.KNEADING_METHODS.has(method)) return;
    this.filteredRecipes = this.filteredRecipes.filter(r => r.kneadingMethod === method);
  }

  filterByFlourType(type) {
    if (!this.FLOUR_TYPES.has(type)) return;
    this.filteredRecipes = this.filteredRecipes.filter(r => r.flourType === type);
  }

  filterByCuisine(cuisine) {
    if (!this.CUISINES.has(cuisine)) return;
    this.filteredRecipes = this.filteredRecipes.filter(r => r.cuisine === cuisine);
  }

  filterByDoughType(dough) {
    if (!this.DOUGH_TYPES.has(dough)) return;
    this.filteredRecipes = this.filteredRecipes.filter(r => r.doughType === dough);
  }

  sortFilteredBy(criterion, ascending = true) {
    const cmp = this.SORTING_CRITERIA.get(criterion);
    if (!cmp) return;
    this.filteredRecipes.sort(cmp);
    if (!ascending) this.filteredRecipes.reverse();
  }
}

module.exports = { Service };
