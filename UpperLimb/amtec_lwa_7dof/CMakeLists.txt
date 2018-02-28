cmake_minimum_required( VERSION 2.8 )
cmake_policy( VERSION ${CMAKE_VERSION} )

project( amtec_lwa_7dof )

#--------------------------------------------------------------------------------------------
# amtec lwas 7dof drivers

set(AMTEC_LWA_7DOF_ROOT $ENV{AMTEC_LWA_7DOF_ROOT})
include_directories(
	${AMTEC_LWA_7DOF_ROOT}/include
)
link_directories(
	${AMTEC_LWA_7DOF_ROOT}/lib
)
#--------------------------------------------------------------------------------------------
#project source and include files
set( SOURCE_FILES
	AmtecLWA7dof.cpp
)

set( HEADER_FILES
	AmtecLWA7dof.h
	Amtec_Definitions.h
) 

#include_directories(common)
#link_directories(lib)

#--------------------------------------------------------------------------------------------

SET( myApp_SRCS
  ${SOURCE_FILES}
  ${HEADER_FILES}
 )
 
source_group( "Header Files" FILES ${HEADER_FILES})
source_group( "Source Files" FILES ${SOURCE_FILES})

add_library( amtec_lwa_7dof STATIC ${myApp_SRCS} )

#target_link_libraries(amtec_lwa_7dof m5apiw32.lib ${YARP_LIBRARIES})
target_link_libraries(amtec_lwa_7dof ${AMTEC_LWA_7DOF_ROOT}/lib/m5apiw32.lib)

# Expose this's public includes (including eigen, tbb and ipp transitively) to other
# subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								 ${AMTEC_LWA_7DOF_ROOT}/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
	
set(${PROJECT_NAME}_LIBRARY_DIRS 
								${AMTEC_LWA_7DOF_ROOT}/lib
    CACHE INTERNAL "${PROJECT_NAME}: Libraries Directories" FORCE)
