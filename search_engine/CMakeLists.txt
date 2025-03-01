project ("Skillbox_GraduationProj")

FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)


add_executable (Skillbox_GraduationProj 
${SOURCE_WAY}/main.cpp 
${HEADERS_WAY}/ConverterJSON.h	${SOURCE_WAY}/ConverterJSON.cpp
${HEADERS_WAY}/InvertedIndex.h	${SOURCE_WAY}/InvertedIndex.cpp 
"headers/SearchServer.h"	"sources/SearchServer.cpp")

target_include_directories(Skillbox_GraduationProj PUBLIC ${HEADERS_WAY})

target_link_libraries(Skillbox_GraduationProj PRIVATE nlohmann_json::nlohmann_json)
