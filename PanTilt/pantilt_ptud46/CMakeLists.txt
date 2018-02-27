cmake_minimum_required( VERSION 2.8 )
cmake_policy( VERSION ${CMAKE_VERSION} )

project( pantilt_ptud46 )
set(CMAKE_BUILD_TYPE RELWITHDEBINFO)

#-------------------------------------------------------------------------------
# Configure Serial Port implementation

find_package( Boost REQUIRED )
set(SERIAL_ASIO_ROOT $ENV{SERIAL_ASIO_ROOT})
#set(SERIAL_ASIO_ROOT C:/serial_asio)
include_directories( 
	${Boost_INCLUDE_DIRS}
	${SERIAL_ASIO_ROOT}/include
)

link_directories( 
	${Boost_LIBRARY_DIRS}
	${SERIAL_ASIO_ROOT}/lib
)


#-------------------------------------------------------------------------------
# Configure preprocessor definition _WIN32_WINNT

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
	# if the target machine is Windows, configure _WIN32_WINNT
	if( ${CMAKE_SYSTEM_VERSION} VERSION_EQUAL 6.2 )
		set( TARGET_OS 602 ) # Windows 8
	elseif( ${CMAKE_SYSTEM_VERSION} VERSION_EQUAL 6.1 )
		set( TARGET_OS 601 ) # Windows 7
	elseif( ${CMAKE_SYSTEM_VERSION} VERSION_EQUAL 6.0 )
		set( TARGET_OS 600 ) # Windows Vista or Server 2008
	elseif( ${CMAKE_SYSTEM_VERSION} VERSION_EQUAL 5.2 )
		set( TARGET_OS 502 ) # Windows Server 2003 with SP1, Windows XP with SP2
	elseif( ${CMAKE_SYSTEM_VERSION} VERSION_EQUAL 5.1 )
		set( TARGET_OS 501 ) # Windows Server 2003, Windows XP
	else()
		set( TARGET_OS 500 ) # Windows 2000 or less
	endif()
		
	add_definitions( -D_WIN32_WINNT=0x0${TARGET_OS} )
endif()

#--------------------------------------------------------------------------------------------
#project source and include files
set( SOURCE_FILES
	pantilt_ptud46.cpp 
)

set( HEADER_FILES
	pantilt_ptud46.h 
	pantilt_definitions.h 
) 

#--------------------------------------------------------------------------------------------

SET( myApp_SRCS
  ${SOURCE_FILES}
  ${HEADER_FILES}
 )
 
source_group( "Header Files" FILES ${HEADER_FILES})
source_group( "Source Files" FILES ${SOURCE_FILES})

#-------------------------------------------------------------------------------
add_library( ${PROJECT_NAME} STATIC ${myApp_SRCS})

target_link_libraries( ${PROJECT_NAME} ${SERIAL_ASIO_ROOT}/lib/serial_asio.lib)

# Expose this's public includes (including eigen, tbb and ipp transitively) to other
# subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								# ${PROJECT_SOURCE_DIR}/common
								${SERIAL_ASIO_ROOT}/include
								 #${YARP_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
	
set(${PROJECT_NAME}_LIBRARY_DIRS 
								${PROJECT_SOURCE_DIR}
								${SERIAL_ASIO_ROOT}/lib
    CACHE INTERNAL "${PROJECT_NAME}: Libraries Directories" FORCE)


