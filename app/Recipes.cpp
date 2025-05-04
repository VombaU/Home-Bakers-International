#include "Recipes.h"
#include <iostream>
static std::chrono::system_clock::time_point FromIso8601(std::string str) {
    std::chrono::system_clock::time_point timestamp;
    std::istringstream ss(str);
    ss >> std::chrono::parse("%FT%TZ", timestamp);
    if (ss.fail()) {
        throw std::invalid_argument("Failed to parse ISO 8601 string");
    }
    return timestamp;
}

namespace Backend {
    Json::Value Recipe::ToJson() {
        Json::Value root;

        root["id"] = Id;
        root["title"] = Title;
        root["description"] = Description;
        root["thumbnailUrl"] = ThumbnailUrl;
        root["rating"] = Rating;
        root["uploadDate"] = std::format("{:%FT%TZ}", UploadDate);
        root["modifiedDate"] = ModifiedDate.has_value() ? static_cast<Json::Value>(std::format("{:%FT%TZ}", ModifiedDate.value())) : Json::nullValue;
        root["cuisine"] = Cuisine;
        root["doughType"] = DoughType;
        root["flourType"] = FlourType;
        root["kneadingMethod"] = KneadingMethod;
        
        return root;
    }

    Recipe Recipe::FromJson(Json::Value root) {
        return Recipe(
            root["id"].asInt(),
            root["title"].asString(),
            root["description"].asString(),
            root["thumbnailUrl"].asString(),
            root["rating"].asDouble(),
            root["cuisine"].asString(),
            root["doughType"].asString(),
            root["flourType"].asString(),
            root["kneadingMethod"].asString(),
            FromIso8601(root["uploadDate"].asString()),
            root["modifiedDate"].isNull() ? std::nullopt : std::optional(FromIso8601(root["modifiedDate"].asString()))
        );
    }

    Recipe::Recipe(int id, std::string title, std::string description, std::string thumbnailUrl, double rating, std::string cuisine, std::string doughType, std::string flourType, std::string kneadingMethod,
        std::chrono::system_clock::time_point uploadDate, std::optional<std::chrono::system_clock::time_point> modifiedDate) {
        Description = description;
        DoughType = doughType;
        FlourType = flourType;
        Id = id;
        KneadingMethod = kneadingMethod;
        ModifiedDate = modifiedDate;
        UploadDate = uploadDate;
        Rating = rating;
        ThumbnailUrl = thumbnailUrl;
        Title = title;
        Cuisine = cuisine;
    }
}