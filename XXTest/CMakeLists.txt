cmake_minimum_required(VERSION 2.6)
project(XXTEST)  
include(CMakePrj/ConfigParse.cmake)
include(CMakePrj/source.txt)

message(STATUS "Source DIR:" ${XXTEST_SOURCE_DIR})

# Base compile define.	
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -lstdc++ -lpthread -g")
if(WIN32) 
	#set (CMAKE_CXX_FLAGS_RELEASE "/MT")
	set(CMAKE_CXX_FLAGS_RELEASE
      	"${CMAKE_CXX_FLAGS_RELEASE}  /MD /Zi /Od /Ob2 /RTC1"
      	CACHE STRING "Release builds CMAKE CXX flags " FORCE )
endif(WIN32)

if(APPLE)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif(APPLE)

# Binary output path and binary name.
set(EXECUTABLE_OUTPUT_PATH ${XXTEST_SOURCE_DIR}/../bin)
	
# Compile define.	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CONFIG_CXX_FLAGS}")

# Include path and link path.
include_directories(${CONFIG_INCLUDE_PATH})
link_directories(${CONFIG_LINK_PATH})
if(UNIX)
	foreach(libpath ${CONFIG_LINK_PATH})
		set(rpath_str, "-Wl,--rpath=${libpath}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${rpath_str}")
	endforeach(libpath ${CONFIG_LINK_PATH})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(UNIX)

# Source files.
set(ALL_LIST 
	${SOURCE_ALL_LIST} 
	${SOURCE_HEADER_LIST})

# exe
add_executable(XXTest ${ALL_LIST})

# Add library.
if(APPLE)
	SET(GUI_TYPE MACOSX_BUNDLE)
	INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(QUICKTIME_LIBRARY QuickTime )
	FIND_LIBRARY(FOUNDATION_LIBRARY Foundation )
	FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
	MARK_AS_ADVANCED (CARBON_LIBRARY
		     QUICKTIME_LIBRARY
		     APP_SERVICES_LIBRARY)
	SET(EXTRA_LIBS ${CARBON_LIBRARY} ${QUICKTIME_LIBRARY} ${APP_SERVICES_LIBRARY} ${FOUNDATION_LIBRARY})	
	target_link_libraries(XXTest ${EXTRA_LIBS} ${CONFIG_LINK_LIBRARY})
else()
	target_link_libraries(XXTest ${CONFIG_LINK_LIBRARY})
endif(APPLE)


