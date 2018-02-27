cmake_minimum_required( VERSION 2.8 )
cmake_policy( VERSION ${CMAKE_VERSION} )

project( barrett_hand_826X )

set(SERIAL_ASIO_ROOT $ENV{SERIAL_ASIO_ROOT})
find_package( Boost REQUIRED )

include_directories( 
	${Boost_INCLUDE_DIRS}
	${SERIAL_ASIO_ROOT}/include
)

link_directories( 
	${Boost_LIBRARY_DIRS}
	${SERIAL_ASIO_ROOT}/lib
)


#--------------------------------------------------------------------------------------------
#project source and include files
set( SOURCE_FILES
	barrett_hand_826X.cpp
)

set( HEADER_FILES
	barrett_hand_826X.h
	barrett_hand_definitions.h
) 

#--------------------------------------------------------------------------------------------

SET( myApp_SRCS
  ${SOURCE_FILES}
  ${HEADER_FILES}
 )
 
source_group( "Header Files" FILES ${HEADER_FILES})
source_group( "Source Files" FILES ${SOURCE_FILES})

add_library( ${PROJECT_NAME} STATIC ${myApp_SRCS} )

#target_link_libraries(barrett_hand_826X  serial_asio ${YARP_LIBRARIES})
target_link_libraries(${PROJECT_NAME}  ${SERIAL_ASIO_ROOT}/lib/serial_asio.lib)

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

