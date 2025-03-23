using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace Frontend.Models
{
    public class Recipe
    {
        public int Id { get; set; }

        [Required(ErrorMessage = "Title is required")]
        [MaxLength(100, ErrorMessage = "Title cannot exceed 100 characters")]
        public string Title { get; set; } = string.Empty;

        [Required(ErrorMessage = "Description is required")]
        [MaxLength(1000, ErrorMessage = "Description cannot exceed 1000 characters")]
        public string Description { get; set; } = string.Empty;

        [Required(ErrorMessage = "Thumbnail URL is required")]
        [Url(ErrorMessage = "Please enter a valid URL")]
        public string ThumbnailUrl { get; set; } = string.Empty;

        [Required(ErrorMessage = "Rating is required")]
        [Range(0.5, 5, ErrorMessage = "Rating must be between 0.5 and 5")]
        public double Rating { get; set; }

        public DateTime UploadDate { get; set; }
        public DateTime? ModifiedDate { get; set; }
        
        // Tags (one per category)
        [Required(ErrorMessage = "Cuisine is required")]
        public string Cuisine { get; set; } = string.Empty;

        [Required(ErrorMessage = "Dough Type is required")]
        public string DoughType { get; set; } = string.Empty;

        [Required(ErrorMessage = "Flour Type is required")]
        public string FlourType { get; set; } = string.Empty;

        [Required(ErrorMessage = "Kneading Method is required")]
        public string KneadingMethod { get; set; } = string.Empty;
    }

    public class Ingredient
    {
        public string Name { get; set; }
        public decimal Amount { get; set; }
        public string Unit { get; set; }
    }
} 