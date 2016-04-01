# CMake entry point
cmake_minimum_required (VERSION 2.6)
project(polytech_ricm4_tp7)

find_package(OpenGL REQUIRED)


find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

# Compile external dependencies 
#add_subdirectory (external)
add_subdirectory (./external)
#add_subdirectory (./../../../../external ./../../../../external/build)

include_directories(src/)
include_directories(
#	./../../../external/glfw-2.7.6/include/
#	./../../../external/glm-0.9.4.0/
#	./../../../external/glew-1.9.0/include/
	./external/glfw-2.7.6/include/
	./external/glm-0.9.4.0/
	./external/glew-1.9.0/include/
)


set(ALL_LIBS
	GLFW_276
	GLEW_190
	${QT_LIBRARIES}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS

)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(
    GLOB
    shader_file
    shader/*.glsl
    )

file(
	GLOB_RECURSE
	source_files
        src/*.cpp
        src/*.h
)

add_executable(polytech_ricm4_tp7  ${shader_file} ${source_files} )
target_link_libraries(polytech_ricm4_tp7 ${ALL_LIBS})

