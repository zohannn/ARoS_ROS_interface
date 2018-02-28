#-------------------------------------------------------------------------------
# CMake minimum required version and used policy
cmake_minimum_required( VERSION 2.8 )
cmake_policy( VERSION ${CMAKE_VERSION} )

#-------------------------------------------------------------------------------
project( PanTilt )

#-------------------------------------------------------------------------------
# Test environment variable MAINPROJECT
if( DEFINED ENV{MAINPROJECT} )

	set( MAIN_PROJECT $ENV{MAINPROJECT} )
	set( LIBRARIES_DIR ${MAIN_PROJECT}/Libraries )

	include( ${LIBRARIES_DIR}/CMakeScripts/CommonScripts.cmake )

else()
	message( SEND_ERROR "Environment variable 'MAINPROJECT' not defined!" )
endif()

#-------------------------------------------------------------------------------
# Define global MSVC heap allocation parameters
Setup_Global_MSVC_Heap_Allocation( ON 0 )

set(PANTILT_PTUD46_ROOT $ENV{PANTILT_PTUD46_ROOT})
#set(PANTILT_PTUD46_ROOT C:/pantilt_ptud46/install)
include_directories( 
	${PANTILT_PTUD46_ROOT}/include
	${pantilt_ptud46_INCLUDE_DIRS}
)

link_directories( 
	${PANTILT_PTUD46_ROOT}/lib
	${pantilt_ptud46__LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Configure Serial Port implementation

find_package( Boost REQUIRED)

include_directories( 
	${Boost_INCLUDE_DIRS} 
)

link_directories( 
 ${Boost_LIBRARY_DIRS}
)

#--------------------------------------------------------------------------------------------
#project source and include files
set( SOURCE_FILES
	PanTilt.cpp
	CommunicationServerPanTilt.cpp
	EnvironmentVars.cpp
	Files_Directories.cpp
	string_operations.cpp
)

set( HEADER_FILES
	PanTiltWrapper_defs.h
	CommunicationServerPanTilt.h
	Files_Directories.h
	EnvironmentVars.h
	MathDefines.h
	ansi_unicode_definitions.h
	string_operations.h
) 

#-------------------------------------------------------------------------------
	

SET( myApp_SRCS
  ${SOURCE_FILES}
  ${HEADER_FILES}
 )

add_library( ${PROJECT_NAME} STATIC ${myApp_SRCS} )

target_link_libraries(${PROJECT_NAME}  ${PANTILT_PTUD46_ROOT}/lib/pantilt_ptud46.lib)

# Expose this's public includes (including eigen, tbb and ipp transitively) to other
# subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								${PANTILT_PTUD46_ROOT}/include
								${pantilt_ptud46_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
	
set(${PROJECT_NAME}_LIBRARY_DIRS 
								${PROJECT_SOURCE_DIR}
								${PANTILT_PTUD46_ROOT}/lib
								${pantilt_ptud46_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Libraries Directories" FORCE)
