cmake_minimum_required(VERSION 3.16.0)
project(FSD LANGUAGES C CXX)

# Avoid mid-level directory to be created under MSVC
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" CACHE INTERNAL "Where to place executables and dlls")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin" CACHE INTERNAL "Where to place executables and dlls in release mode")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}/bin")


set(SubmoduleDir "${CMAKE_SOURCE_DIR}/ext")
set(FX3DRoot "${SubmoduleDir}/FluidX3D")
set(JSONRoot "${SubmoduleDir}/json")

add_subdirectory("${FX3DRoot}")
add_subdirectory("${JSONRoot}")


include_directories("${CMAKE_SOURCE_DIR}/include")

# Executable
set(SIM_SRC 
    "src/simulate_scene.cpp" 
    "src/dataset_scene.cpp" 
    "src/droplets.cpp" 
    "src/ballistic.cpp" 
    "src/nerf_scene.cpp"
    )

add_executable(SimulateScene ${SIM_SRC})
set_property(TARGET SimulateScene PROPERTY CXX_STANDARD 17) 
target_link_libraries(SimulateScene FX3D nlohmann_json)
target_include_directories(SimulateScene PUBLIC "${FX3DRoot}/include" "${JSONRoot}/include")
