#pragma once
#include <string>
#include <json/json.h>
#include <chrono>
#include <optional>
namespace Backend
{
    class Recipe {
    public:
        int Id;
        //[Required(ErrorMessage = "Title is required")]
        //[MaxLength(100, ErrorMessage = "Title cannot exceed 100 characters")]
        std::string Title;

        //[Required(ErrorMessage = "Description is required")]
        //[MaxLength(1000, ErrorMessage = "Description cannot exceed 1000 characters")]
        std::string Description;

        //[Required(ErrorMessage = "Thumbnail URL is required")]
        //[Url(ErrorMessage = "Please enter a valid URL")]
        std::string ThumbnailUrl;

        //[Required(ErrorMessage = "Rating is required")]
        //[Range(0.5, 5, ErrorMessage = "Rating must be between 0.5 and 5")]
        double Rating;

        std::chrono::system_clock::time_point UploadDate;
        std::optional<std::chrono::system_clock::time_point> ModifiedDate;

        // Tags (one per category)
        //[Required(ErrorMessage = "Cuisine is required")]
        std::string Cuisine;

        //[Required(ErrorMessage = "Dough Type is required")]
        std::string DoughType;

        //[Required(ErrorMessage = "Flour Type is required")]
        std::string FlourType;

        //[Required(ErrorMessage = "Kneading Method is required")]
        std::string KneadingMethod;

        Recipe(int id, std::string title, std::string description, std::string thumbnailUrl, double rating, std::string cuisine, std::string doughType, std::string flourType, std::string KneadingMethod,
            std::chrono::system_clock::time_point uploadDate, std::optional<std::chrono::system_clock::time_point> modifiedDate = std::nullopt);

        Json::Value ToJson();

        static Recipe FromJson(Json::Value root);
    };
}