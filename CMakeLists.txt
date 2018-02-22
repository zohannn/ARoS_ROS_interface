#--------------------------------------------------------------------------------------------
# Define CMake policy to be used on the project
cmake_minimum_required( VERSION 2.4 )
cmake_policy( VERSION ${CMAKE_VERSION} )
#--------------------------------------------------------------------------------------------
# The project name
project(ARoS_ROS_interface)

set( HOME $ENV{MAINPROJECT} )
#set(HOME C:/Work/Repository)
set( LIB_BIN_DIR C:/Work/Repository/Libraries_bin )

include( ${LIB_BIN_DIR}/CMakeScripts/CommonScripts.cmake )

#--------------------------------------------------------------------------------------------
# ROS and ROSDEPS directories
set( ROS_INCLUDE $ENV{ROS_INCLUDE} )
set( ROS_LIB $ENV{ROS_LIB} )
set( ROSDEPS_INCLUDE $ENV{ROSDEPS_INCLUDE} )
set( ROSDEPS_LIB $ENV{ROSDEPS_LIB} )
#--------------------------------------------------------------------------------------------
# Boost Library
find_package(Boost)

#--------------------------------------------------------------------------------------------
# Define Variables to store directories that will be used on the project
	set( LIBRARIES_DIR ${HOME}/Libraries )
	set( HARDWARE_DIR ${HOME}/Hardware )

#--------------------------------------------------------------------------------------------
# Configuration of necessary packages	
	
	#Configure_Package( YARP )
	
#--------------------------------------------------------------------------------------------
# Other projects that are required to be compiled


	#set( MatrixBin_DIR ${LIB_BIN_DIR}/Matrix )
	#find_package( MatrixBin REQUIRED )

	#set( YarpCommunicationBin_DIR ${LIB_BIN_DIR}/YarpCommunication )
	#find_package( YarpCommunicationBin REQUIRED )

#--------------------------------------------------------------------------------------------
# Definition of include directories

	# Directories with code to be compiled
	set( SRC_DIRS 
		${CMAKE_SOURCE_DIR} /src
	)
	set( INCLUDE_DIRS 
		${CMAKE_SOURCE_DIR} /include
	)

	include_directories( 
		${SRC_DIRS}
		${INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/res
		${ROS_INCLUDE}
		${ROSDEPS_INCLUDE}
		${Boost_INCLUDE_DIRS}
		#${LIBRARIES_DIR}/TypesDefs
		#${LIBRARIES_DIR}/Utilities

	)
	
	link_directories(
		${ROS_LIB}
		${ROSDEPS_LIB}
		${Boost_LIBRARY_DIRS}
	)
		
#--------------------------------------------------------------------------------------------
# Create groups of files: Header, Source and Resource files
	
	set( SOURCE_EXTENSIONS *.cpp )
	set( HEADER_EXTENSIONS *.h  *.hpp)

	# Look through all external source directories for source and header files
	foreach( source_dir IN ITEMS ${SRC_DIRS} )

		# Find source files in directories
		foreach( source_ext IN ITEMS ${SOURCE_EXTENSIONS} )
			file( GLOB_RECURSE FOUND_FILES ${source_dir}/${source_ext} )
			set( SOURCE_FILES ${SOURCE_FILES} ${FOUND_FILES} )
		endforeach( source_ext IN ITEMS ${SOURCE_EXTENSIONS} )
	endforeach( source_dir IN ITEMS ${SRC_DIRS} )
		
	foreach( source_dir IN ITEMS ${INCLUDE_DIRS} )
		# Find header files in directories
		foreach( header_ext IN ITEMS ${HEADER_EXTENSIONS} )
			file( GLOB_RECURSE FOUND_FILES ${source_dir}/${header_ext} )
			set( HEADER_FILES ${HEADER_FILES} ${FOUND_FILES} )
		endforeach( header_ext IN ITEMS ${HEADER_EXTENSIONS} )

	endforeach( source_dir IN ITEMS ${INCLUDE_DIRS} )
	

	# Create a list with resource files
	file( GLOB RESOURCE_FILES "res/*.rc*" "res/*.ico" )
	
	Create_File_Groups(  "${HEADER_FILES}" "${SOURCE_FILES}" "${RESOURCE_FILES}" )

#--------------------------------------------------------------------------------------------
# Define what files to be compiled for the application
	set( Project_App_SRCS
		${SOURCE_FILES}
		${HEADER_FILES}
		${RESOURCE_FILES}
	)

#--------------------------------------------------------------------------------------------
# Configure the compiler with the appropriate definitions to the used version
	Configure_WINVER()
	
	# disable function deprecation warning
	add_definitions( -D_CRT_NONSTDC_NO_DEPRECATE )

	Display_Config_Message()
	
#--------------------------------------------------------------------------------------------
# Preprocessor definitions
	
	foreach( ext_pkg IN ITEMS ${EXTERNAL_PACKAGES} )
		# General dependency from project to each external package
		# Create a preprocessor definition that specifies if that external package is used
		if( ${ext_pkg}_FOUND )
			add_definitions( -DUSE_${ext_pkg} )
		endif( ${ext_pkg}_FOUND )
	endforeach( ext_pkg IN ITEMS ${EXTERNAL_PACKAGES} )

	Configure_MFC() # comment this line for console program
#--------------------------------------------------------------------------------------------
# Create an executable for the application
	add_executable( ${PROJECT_NAME} WIN32 ${Project_App_SRCS} )
	# add_executable( ${PROJECT_NAME} ${Project_App_SRCS} ) #Console program
#--------------------------------------------------------------------------------------------
# Configure project dependencies of other projects and respective libraries
	
	#add_definitions( -DUSE_YarpCommunication )
	#add_dependencies( ${PROJECT_NAME} YarpCommunication )
	
#--------------------------------------------------------------------------------------------
# Required Libraries to be linked with the executable
	
	target_link_libraries( ${PROJECT_NAME}
		roscpp.lib
		roslib.lib
		rostime.lib
		rosconsole.lib
		roscpp_serialization.lib
	)
