cmake_minimum_required(VERSION 3.16.0)
project(FSD LANGUAGES C CXX)



set(FX3DRoot "${CMAKE_SOURCE_DIR}/ext/FluidX3D")
set(jsonRoot "${CMAKE_SOURCE_DIR}/ext/json")

add_subdirectory(${FX3DRoot})
add_subdirectory(${jsonRoot})



# Executable
set(SRC_FILES "src/main.cpp")

add_executable(FSDGen ${SRC_FILES})
set_property(TARGET FSDGen PROPERTY CXX_STANDARD 17) 
target_link_libraries(FSDGen FX3D nlohmann_json)
target_include_directories(FSDGen PUBLIC "${FX3DRoot}/include" "${jsonRoot}/include")