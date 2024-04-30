find_package(Boost REQUIRED COMPONENTS filesystem)
if (NOT Boost_FOUND)
    message( FATAL_ERROR "Required Boost packages not found. Perhaps add -DBOOST_ROOT?" )
endif()

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_library(${LIB_BINARY} SHARED ${SOURCES})

set_target_properties(${LIB_BINARY} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${LIB_BINARY} PUBLIC
    Boost::filesystem
)

target_include_directories(${LIB_BINARY} INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    Boost::filesystem
)
