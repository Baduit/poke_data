cmake_minimum_required(VERSION 3.24)

project(local_data_generator VERSION 0.0.1 DESCRIPTION "Generate all the needed data for Pokeguesser from the data of github repository of pokeapi to avoid calling the API everyday")

# Flags
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MSVC_FLAGS /W3 /permissive- /TP)
set(GCC_CLANG_FLAGS -Wextra -Wall -Wsign-conversion -Wfloat-equal -pedantic -Wredundant-decls -Wshadow -Wpointer-arith -O3)

# Depedencies
find_package(strong_type CONFIG REQUIRED)
find_package(nlohmann_json 3.2.0 CONFIG REQUIRED)
find_package(tl-generator CONFIG REQUIRED)
find_package(units CONFIG REQUIRED)

# Executable
set(SRCS
		src/main.cpp
		src/PokemonGenerator.cpp
	)
add_executable(local_data_generator ${SRCS})
target_compile_options(local_data_generator
					   PRIVATE
					   $<$<CXX_COMPILER_ID:MSVC>:${MSVC_FLAGS}>
					   $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:${GCC_CLANG_FLAGS}>)
target_include_directories(local_data_generator PRIVATE src)					   
target_link_libraries(local_data_generator PRIVATE strong_type::strong_type nlohmann_json::nlohmann_json tl::generator units::units)					   
 
