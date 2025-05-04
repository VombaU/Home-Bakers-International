// app.cpp : Defines the entry point for the application.
//

#include "app.h"
#include "Recipes.h"
#include "Service.h"
#include <fstream>
#include <drogon/drogon.h>
#include <drogon/HttpResponse.h>
#include <mysql/mysql.h>
#include <json/json.h>
#include <iostream>
using namespace std;
using namespace Backend;
Service service;
int main()
{
    //Recipe lola;
    //lola.Description = "hmm";
    //lola.DoughType = "wow";
    //lola.FlourType = "hmm";
    //lola.Id = 2;
    //lola.KneadingMethod = "not";
    //lola.ModifiedDate = "ins";
    //lola.UploadDate = "ane";
    //lola.Rating = 2;
    //lola.ThumbnailUrl = "url";
    //lola.Title = "title";
    //lola.Cuisine = "lol";
    //cout << lola.ToJson();
    //cout << service.GetAllAsJson()

    drogon::app().loadConfigFile("C:\\Users\\Tristopher\\OneDrive\\Desktop\\Systems Design\\Home Bakers International\\app\\config.json");
    //drogon::app().addListener("0.0.0.0", 8080);
    drogon::app().registerPreSendingAdvice([](const drogon::HttpRequestPtr& req, const drogon::HttpResponsePtr& resp) {
        if ((req->path().starts_with("/recipes") || req->path().starts_with("/recipe/edit")) && req->method() == drogon::Options) {
            resp->removeHeader("Access-Control-Allow-Origin");
            resp->removeHeader("Access-Control-Allow-Methods");
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, PATCH, POST, DELETE, OPTIONS");
        }
        });

    drogon::app().registerHandler("/recipes", [](const drogon::HttpRequestPtr& req, drogon::AdviceCallback&& callback) {
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        response.get()->setStatusCode(drogon::k402PaymentRequired);
        response.get()->addHeader("Access-Control-Allow-Origin", "*");
        response.get()->addHeader("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
        response.get()->addHeader("Access-Control-Allow-Headers", "*");
        callback(response);
        },
        { drogon::Options });

    drogon::app().registerHandler("/recipes", [](const drogon::HttpRequestPtr& req, drogon::AdviceCallback&& callback) {
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        response.get()->addHeader("Access-Control-Allow-Origin", "*");
        auto id = req.get()->getParameter("id");
        if (id.empty()) {
            response.get()->setStatusCode(drogon::k400BadRequest);
            response.get()->setBody("Error deleting the recipe: no id provided in query params");
            callback(response);
            return;
        }

        try {
            bool success = service.DeleteRecipeById(std::stoi(id));
            if (success) {
                response.get()->setStatusCode(drogon::k204NoContent);
            }
            else {
                response.get()->setStatusCode(drogon::k404NotFound);
                response.get()->setBody("Error deleting the recipe: no recipe found with the provided id");
            }
            callback(response);
        }
        catch (std::exception& err) {
            response.get()->setStatusCode(drogon::k400BadRequest);
            response.get()->setBody(std::string("Error deleting recipe: ") + err.what());
            callback(response);
        }
        },
        { drogon::Delete });



    drogon::app().registerHandler("/recipes", [](const drogon::HttpRequestPtr& req, drogon::AdviceCallback&& callback) {
        drogon::HttpResponsePtr response;
        auto params = req->getParameters();
        bool bToFilter = false;
        for (auto& param : params) {
            if (param.second != "") {
                bToFilter = true;
            }
        }
        if (bToFilter) {
            service.ResetFilteredRecipes();
            service.FilterRecipesByCuisine(params["cuisine"]);
            service.FilterRecipesByDoughType(params["doughtype"]);
            service.FilterRecipesByFlourType(params["flourtype"]);
            service.FilterRecipesByKneadingMethod(params["kneadingmethod"]);
            service.SortFilteredRecipesBy(params["sortby"], params["ascending"] == "True");
        }
        try {
            response = drogon::HttpResponse::newHttpJsonResponse(bToFilter ? service.GetFilteredRecipesAsJson() : service.GetAllAsJson());
            response.get()->setStatusCode(drogon::k200OK);
            response.get()->addHeader("Access-Control-Allow-Origin", "*");
            callback(response);
        }
        catch (drogon::HttpException& err) {
            response = drogon::HttpResponse::newHttpResponse();
            response.get()->setStatusCode(drogon::k500InternalServerError);
            response.get()->setBody(std::string("Drogon Exception: ") + err.what());
            callback(response);
        }
        },
        { drogon::Get });

    drogon::app().registerHandler("/recipe/new", [](const drogon::HttpRequestPtr& req, drogon::AdviceCallback&& callback) {
        Json::Value root;
        Json::Reader reader;
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        response.get()->addHeader("Access-Control-Allow-Origin", "*");
        if (!reader.parse(req.get()->bodyData(), root)) {
            response.get()->setStatusCode(drogon::k400BadRequest);
            response.get()->setBody("Frontend Exception: Invalid JSON");
            callback(response);
            return;
        }

        try {
            service.AddRecipe(Recipe::FromJson(root));
            response.get()->setStatusCode(drogon::k201Created);
            callback(response);
        }
        catch (std::exception& err) {
            response.get()->setStatusCode(drogon::k400BadRequest);
            response.get()->setBody(std::string("Error creating recipe: ") + err.what());
            callback(response);
        }
        },
        { drogon::Post });

    drogon::app().registerHandler("/recipe/new", [](const drogon::HttpRequestPtr& req, drogon::AdviceCallback&& callback) {
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        response.get()->setStatusCode(drogon::k200OK);
        response.get()->addHeader("Access-Control-Allow-Origin", "*");
        response.get()->addHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
        response.get()->addHeader("Access-Control-Allow-Headers", "*");
        callback(response);
        },
        { drogon::Options });

    drogon::app().registerHandler("/recipe/edit/{id}", [](const drogon::HttpRequestPtr& req, drogon::AdviceCallback&& callback, int id) {
        Json::Value root;
        Json::Reader reader;
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        if (!reader.parse(req->bodyData(), root)) {
            response->setStatusCode(drogon::k400BadRequest);
            response->setBody("Frontend Exception: Invalid JSON");
            callback(response);
            return;
        }
        std::cout << root;
        /// TODO: CHECK IF ID PASSED BY PATH PARAM IS EQUAL TO THAT OF THE RECIPE OBJECT
        try {
            bool success = service.UpdateRecipeById(id, Recipe::FromJson(root));
            if (success) {
                response->setStatusCode(drogon::k200OK);
            }
            else {
                response->setStatusCode(drogon::k404NotFound);
                response->setBody("Error updating the recipe: no recipe found with the provided id");
            }
            callback(response);
        }
        catch (std::exception& err) {
            response->setStatusCode(drogon::k400BadRequest);
            response->setBody(std::string("Error updating the recipe: ") + err.what());
            callback(response);
        }
    },
    { drogon::Patch });

    drogon::app().registerHandler("/recipe/edit/{id}", [](const drogon::HttpRequestPtr& req, drogon::AdviceCallback&& callback, int id) {
        drogon::HttpResponsePtr response = drogon::HttpResponse::newHttpResponse();
        response.get()->setStatusCode(drogon::k200OK);
        response.get()->addHeader("Access-Control-Allow-Origin", "*");
        response.get()->addHeader("Access-Control-Allow-Methods", "PATCH, OPTIONS");
        response.get()->addHeader("Access-Control-Allow-Headers", "*");
        callback(response);
        },
        { drogon::Options });

    //
    //MYSQL* connection = mysql_init(NULL);
    //if (!connection) {
    //    LOG_ERROR << "mysql_init failed";
    //    return 1;
    //}
    //mysql_options(connection, MYSQL_OPT_CONNECT_TIMEOUT, "30");
    //mysql_options(connection, MYSQL_OPT_MAX_ALLOWED_PACKET, std::to_string(INT_MAX).c_str());
    //if (!mysql_real_connect(connection, "127.0.0.1", "root", "lol", "recipes", 3306, NULL, 0)) {
    //    LOG_ERROR << "Direct mysql_real_connect failed: Error(" << mysql_errno(connection) << "): " << mysql_error(connection);
    //    mysql_close(connection);
    //    return 1;
    //}
    //mysql_query(connection, "SELECT * FROM devious LIMIT 1");
    //MYSQL_RES* result = mysql_store_result(connection);
    //MYSQL_ROW first = mysql_fetch_row(result);
    //if (mysql_fetch_lengths(result)[0] < 1000) {
    //    mysql_free_result(result);
    //    mysql_close(connection);
    //    LOG_ERROR << "FETCHED THE WRONG SHIT";
    //    return -1;
    //}
    //std::ofstream fout("ambassin.gif", std::ios::binary);
    //fout.write(first[0], mysql_fetch_lengths(result)[0]);
    //fout.close();

    //LOG_INFO << "Gif bytes written to file";
    //mysql_close(connection);


    ////    drogon::app().getDbClient("joe");
    ////LOG_INFO << "Initializing MariaDB client with Drogon...";
    ////DbClientPtr client = DbClient::newMysqlClient("host=127.0.0.1 user=root password=lol dbname=recipes port=3306 connect-timeout=150", 1);
    ////client.get()->closeAll();
    //
    drogon::app().run();
	return 0;
}
