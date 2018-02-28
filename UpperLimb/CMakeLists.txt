cmake_minimum_required( VERSION 2.8 )
cmake_policy( VERSION ${CMAKE_VERSION} )

project( UpperLimb )


#--------------------------------------------------------------------------------------------
#3rd party dependencies
#set(Boost_DEBUG ON)
#set(Boost_USE_STATIC_LIBS ON)
find_package( Boost REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

#--------------------------------------------------------------------------------------------
#libs dependencies
add_subdirectory(amtec_lwa_7dof)
add_subdirectory(barrett_hand_826X)
add_subdirectory(Amtec_PStepServer)

#Project dependencies include directories
include_directories(
	${amtec_lwa_7dof_INCLUDE_DIRS}
	${barrett_hand_826X_INCLUDE_DIRS}
	${Amtec_PStepServer_INCLUDE_DIRS}
)
	


#Project dependencies link directories
link_directories(
	${amtec_lwa_7dof_LIBRARY_DIRS}
	${barrett_hand_826X_LIBRARY_DIRS}
	${Amtec_PStepServer_LIBRARY_DIRS}
)

#--------------------------------------------------------------------------------------------

#Project include directories
include_directories(
	Core
) 

#project source, and include
set( HEADER_FILES
	Core/UpperLimbReqProcessor.h
	Core/Joint_States.h
)

set( SOURCE_FILES 
	Core/UpperLimbReqProcessor.cpp
	Core/UpperLimb.cpp
	Core/UpperLimbArmReqProcessor.cpp
	Core/UpperLimbHandReqProcessor.cpp
	Core/Joint_States.cpp
)

set( myApp_SRCS
  ${SOURCE_FILES}
  ${HEADER_FILES}
 )

source_group( "Header Files" FILES ${HEADER_FILES})
source_group( "Source Files" FILES ${SOURCE_FILES})


#--------------------------------------------------------------------------------------------

# Expose this's public includes (including eigen, tbb and ipp transitively) to other
# subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Core
								${amtec_lwa_7dof_INCLUDE_DIRS}
								${barrett_hand_826X_INCLUDE_DIRS}
								${Amtec_PStepServer_INCLUDE_DIRS}
								# ${PROJECT_SOURCE_DIR}/common
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
	
set(${PROJECT_NAME}_LIBRARY_DIRS 
								${PROJECT_SOURCE_DIR}/lib
    CACHE INTERNAL "${PROJECT_NAME}: Libraries Directories" FORCE)
	
add_library(${PROJECT_NAME} STATIC ${myApp_SRCS})

target_link_libraries(UpperLimb amtec_lwa_7dof barrett_hand_826X Amtec_PStepServer)

#create the executable
#add_executable( UpperLimb ${myApp_SRCS} )

#link to which libs
#target_link_libraries(UpperLimb amtec_lwa_7dof barrett_hand_826X Amtec_PStepServer YarpCommunication ${YARP_LIBRARIES})
