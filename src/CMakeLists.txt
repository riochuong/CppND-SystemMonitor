#
# Set the binary executable name
#
set(project_BIN ${PROJECT_NAME})

#
# Find all executables
#
file(GLOB project_SRCS *.cpp *.h)
#file(ProcessParser.cpp ProcessParser.h)

#
# Add executables to project
#
add_executable(${project_BIN} ${project_SRCS})


#
# create a library for unit tests
#
add_library(process_parser OBJECT ProcessParser.cpp)
add_library(util OBJECT util.cpp)
#target_include_directories(route_planner PRIVATE ../thirdparty/pugixml/src)


if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    target_link_libraries(${project_BIN} PUBLIC ${CURSES_LIBRARIES})
endif()

if(MSVC)
	target_compile_options(${project_BIN} PUBLIC /D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING /wd4459)
endif()
