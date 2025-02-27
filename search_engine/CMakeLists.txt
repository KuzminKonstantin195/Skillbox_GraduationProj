project ("Skillbox_GraduationProj")

FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

#starget_include_directories(Skillbox_GraduationProj PUBLIC headers)

add_executable (Skillbox_GraduationProj "sources/main.cpp" )

target_link_libraries(Skillbox_GraduationProj PRIVATE nlohmann_json::nlohmann_json)
