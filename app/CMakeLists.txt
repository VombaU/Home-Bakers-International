# CMakeList.txt : CMake project for app, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("app")

# Add source to this project's executable.
add_executable (app "app.cpp" "app.h" "Recipes.cpp" "Recipes.h" "Service.cpp" "Service.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET app PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

find_package(Drogon CONFIG REQUIRED)
target_link_libraries(app PRIVATE Drogon::Drogon)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(app PRIVATE OpenSSL::SSL OpenSSL::Crypto)

find_package(ZLIB REQUIRED)
target_link_libraries(app PRIVATE ZLIB::ZLIB)

find_package(unofficial-libmariadb CONFIG REQUIRED)
target_link_libraries(app PRIVATE unofficial::libmariadb)

