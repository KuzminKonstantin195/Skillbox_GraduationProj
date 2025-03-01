project("GtestTests")

set(SrcFiles
	${SOURCE_WAY}/InvertedIndex.cpp
	${SOURCE_WAY}/SearchServer.cpp
	${SOURCE_WAY}/ConverterJSON.cpp
)

set(TestsFiles
	InvertedIndex_test.cpp
	SearchServer_test.cpp
	ConverterJSON_test.cpp
)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

add_executable(GtestTests ${SrcFiles} ${TestsFiles})

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(
  GtestTests PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json
)
include(GoogleTest)

target_include_directories(GtestTests PUBLIC ${HEADERS_WAY})
gtest_discover_tests(GtestTests)
