find_package(Boost REQUIRED COMPONENTS program_options)
if (NOT Boost_FOUND)
    message( FATAL_ERROR "Required Boost packages not found. Perhaps add -DBOOST_ROOT?" )
endif()

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_executable(${INSTALL_TARGET} ${SOURCES})

set_target_properties(${INSTALL_TARGET} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${INSTALL_TARGET} PUBLIC
    ${LIB_BINARY}
    Boost::program_options
)
