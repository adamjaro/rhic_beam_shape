
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(rhic_beam_shape)

#local library
set(LIB rhic_beam_shape)

#sources and headers
file(GLOB sim_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx)
file(GLOB sim_inc ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#BOOST
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#ROOT
find_package(ROOT REQUIRED COMPONENTS Core EG Hist Physics RIO Tree MathCore)
include(${ROOT_USE_FILE})

#build the library
add_library (${LIB} SHARED ${sim_src} ${sim_inc})
target_link_libraries(${LIB} ${Boost_LIBRARIES} ${ROOT_LIBRARIES})

