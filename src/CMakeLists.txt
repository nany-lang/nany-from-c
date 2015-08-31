project(nany-from-c)
cmake_minimum_required(VERSION 2.8)


include("cmake/messages.cmake")

nmessage("Nany Compiler Suite -- Nany From C Converter")
include("cmake/utils.cmake")


if ("${CMAKE_BUILD_TYPE}" STREQUAL "" OR (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "release" AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "debug"))
	# release by default
	set(CMAKE_BUILD_TYPE "debug")
endif()
nmessage("configuration: ${CMAKE_BUILD_TYPE}")



set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")


# Yuni
# ----
if(NOT YUNI_CONFIG)
	set(YUNI_CONFIG yuni-config)
endif()
nmessage("${YUNI_CONFIG}")
set(YUNI_MODULES -m uuid)
execute_process(COMMAND "${YUNI_CONFIG}" ${YUNI_MODULES} --cxxflags
				OUTPUT_VARIABLE YUNI_CXXFLAGS
				OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${YUNI_CONFIG}" ${YUNI_MODULES} --libs
				OUTPUT_VARIABLE YUNI_LIBS
				OUTPUT_STRIP_TRAILING_WHITESPACE)
if ("${YUNI_CXXFLAGS}" STREQUAL "" OR "${YUNI_LIBS}" STREQUAL "")
	nerror("yuni-config was not found ! Please install Yuni (or set -DYUNI_CONFIG to specify the yuni-config location).")
endif()


# Clang
# -----
#set(LIBCLANG_INCLUDES "-Iext -Iext/clang")
#set(LIBCLANG_LIBS "ext/lib/libclang.lib")

include("cmake/FindLibClang.cmake")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${YUNI_CXXFLAGS} -I. -Wall -Wextra -W -Wno-unused-parameter -DDEBUG -ggdb3 ${INCLUDES} -I${LIBCLANG_INCLUDES}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${YUNI_CXXFLAGS} -I. -Wall -Wextra -W -Wno-unused-parameter -DNDEBUG -O3 ${INCLUDES} -I${LIBCLANG_INCLUDES}")
link_libraries("${YUNI_LIBS} ${LIBCLANG_LDFLAGS}")



#
# config.h
#
nmessage("generating config.h")
configure_file("cmake/config.h.cmake" "config.h")



# Parser for C files
# add_subdirectory(parser)

# Nany exporter
# add_subdirectory(writer)

add_executable(nany-from-c

	main.cpp
	converter.cpp
	converter.h
	parser/parser.cpp
	parser/parser.h
	writer/visitor.cpp
	writer/visitor.h
)

target_link_libraries(nany-from-c "${YUNI_LIBS}")


if(nany-from-c_error_has_occured)
	message(STATUS "")
	message(STATUS "")
	if(UNIX)
		message(SEND_ERROR "[1;31mAn error has occured. Aborting. [0m")
	else()
		message(SEND_ERROR "An error has occured. Aborting.")
	endif()
endif()

# Empty message, for beauty :)
nmessage("") # done
