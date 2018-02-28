#-------------------------------------------------------------------------------
# CMake minimum required version and used policy
cmake_minimum_required( VERSION 2.8 )
cmake_policy( VERSION ${CMAKE_VERSION} )
#-------------------------------------------------------------------------------
# The project name
project( VisionBot )
set(CMAKE_BUILD_TYPE RELWITHDEBINFO)

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

#-------------------------------------------------------------------------------
# Test if target machine is running Windows
Enforce_Supported_OS( Windows )

#-------------------------------------------------------------------------------
# Mandatory packages to be used

set( EXTERNAL_PACKAGES
	# Main packages
	OpenCV
	#YARP
	# Library packages
	AmariField2D
	StereoVidere
	DataShare
	DialogFileOpenSave
	FilesDirs
	MathLib
	MutexWrapper
	TimeCount
	Utilities
	# YarpCommunication
	# Image Libs packages
	BlobTracker
	ImageIO
	ImageUtils
	MovementAnalysis
)

Configure_Package_List( ${EXTERNAL_PACKAGES} )

# #--------------------------------------------------------------------------
# # OpenCV
# if(OpenCV_FOUND)
	# include_directories( ${OpenCV_INCLUDE_DIRS} )
	# #Add_to_link_libraries( ${OpenCV_LIBS} )
	# set_property( GLOBAL APPEND PROPERTY LINK_LIBRARIES ${OpenCV_LIBS} )
# endif()

#--------------------------------------------------------------------------------------------
#libs dependencies
# add_subdirectory(AmariField2D)
# add_subdirectory(StereoVidere)
# add_definitions( -DUSE_AmariField2D )
# add_definitions( -DUSE_StereoVidere )
# #Project dependencies include directories
# include_directories(
	# ${AmariField2D_INCLUDE_DIRS}
	# ${StereoVidere_INCLUDE_DIRS}
# )

# #Project dependencies link directories
# link_directories(
	# ${AmariField2D_LIBRARY_DIRS}
	# ${StereoVidere_LIBRARY_DIRS}
# )

#-------------------------------------------------------------------------------
# List with optional packages
#set( OPTIONAL_PROJECTS_LIST
#	AmariField2D
#	StereoVidere
#)

#foreach( ext_proj ${OPTIONAL_PROJECTS_LIST} )

	# set( ${PROJECT_NAME}_EXTERNAL_PROJECT_${ext_proj} OFF CACHE BOOL "Use external project ${ext_proj} in ${PROJECT_NAME}" )
	# if( ${PROJECT_NAME}_EXTERNAL_PROJECT_${ext_proj} )
		# set( USED_OPTIONAL_PROJECTS ${USED_OPTIONAL_PROJECTS} ${ext_proj} )
		# Configure_Package( ${ext_proj} )
	# endif()

# endforeach()

#-------------------------------------------------------------------------------
# Boost. Configure_Package doesn't work.
find_package( Boost )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
		
# if( DEFINED ENV{Boost_ROOT} )
	# set( USE_Boost ON CACHE BOOL "Link Boost in ${PROJECT_NAME}? Only required if YARP uses Boost, to fix dependencies." )
	# if( USE_Boost )
		# find_package( Boost )
		# include_directories( ${Boost_INCLUDE_DIRS} )
		# link_directories( ${Boost_LIBRARY_DIRS} )
	# endif()
# endif()

#-------------------------------------------------------------------------------
# Directories with source code to be compiled
set( SRC_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR} 
)

#-------------------------------------------------------------------------------
# pantilt_ptud46
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
# Definition of include directories
SubDirList( SUB_DIRECTORIES_LIST ${CMAKE_CURRENT_SOURCE_DIR} )

include_directories( 
	${SRC_DIRS}
	${SUB_DIRECTORIES_LIST}
	#${LIBRARIES_DIR}/TypesDefs
)
	
#-------------------------------------------------------------------------------
# Create groups of files: Header, Source and Resource files

set( HEADER_EXTENSIONS *.h *.hpp )
set( SOURCE_EXTENSIONS *.cpp )

# Look through all external source directories for source and header files
foreach( source_dir ${SRC_DIRS} )

	# Find source files in directories
	foreach( source_ext ${SOURCE_EXTENSIONS} )
		file( GLOB_RECURSE FOUND_FILES ${source_dir}/${source_ext} )
		set( SOURCE_FILES ${SOURCE_FILES} ${FOUND_FILES} )
	endforeach()
	
	# Find header files in directories
	foreach( header_ext ${HEADER_EXTENSIONS} )
		file( GLOB_RECURSE FOUND_FILES ${source_dir}/${header_ext} )
		set( HEADER_FILES ${HEADER_FILES} ${FOUND_FILES} )
	endforeach()

endforeach()

# Create a list with resource files
#file( GLOB RESOURCE_FILES res/*.rc* res/*.ico )

# Exclude all source and header files in build folder
#file( GLOB_RECURSE FILES_TO_REMOVE ${CMAKE_CURRENT_BINARY_DIR}/*.* )
#list( REMOVE_ITEM HEADER_FILES ${FILES_TO_REMOVE} )
#list( REMOVE_ITEM SOURCE_FILES ${FILES_TO_REMOVE} )

#Create_File_Groups(  "${HEADER_FILES}" "${SOURCE_FILES}" "${RESOURCE_FILES}" )
Create_File_Groups(  "${HEADER_FILES}" "${SOURCE_FILES}" )

#-------------------------------------------------------------------------------
# Define what files to be compiled for the application
set( Project_App_SRCS
	${SOURCE_FILES}
	${HEADER_FILES}
	#${RESOURCE_FILES}
)

#-------------------------------------------------------------------------------

Display_Config_Message()

#-------------------------------------------------------------------------------
# Setup compiler definitions

add_definitions( 
	-D_CRT_NONSTDC_NO_DEPRECATE	# disable function deprecation warning
	-DVISIONBOT_APP_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

# Configure the compiler with the appropriate definitions to the used version
Configure_Windows_OS_Specific_Flags()

# Configure the use of MFC's
#Configure_MFC()

# Workaround to allow the compilation of code containing 'nullptr' when not supported
Handle_nullptr_compatibility()

# Configure compiler to use ANSI or unicode
Configure_Character_Encoding()

# Configure Visual Studio heap allocation
Configure_MSVC_Heap_Allocation()

#-------------------------------------------------------------------------------
# Configuration of necessary flags

# set( EXTRA_FLAGS
	# NVIDIA_CUDA
	# OPENCV_QT
# )

# foreach( extra_flag ${EXTRA_FLAGS} )

	# set( USE_${extra_flag} OFF CACHE BOOL "Use ${extra_flag} in the build?" )

	# if( USE_${extra_flag} )
		# add_definitions( -DUSE_${extra_flag} )
	# endif()

# endforeach()

#-------------------------------------------------------------------------------
# Create an executable for the application
#add_executable( ${PROJECT_NAME} WIN32 ${Project_App_SRCS} )

# Expose this's public includes (including eigen, tbb and ipp transitively) to other
# subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Core
								${PROJECT_SOURCE_DIR}/Comms
								${PROJECT_SOURCE_DIR}/DataStructures
								${PROJECT_SOURCE_DIR}/Dialogs
								${PROJECT_SOURCE_DIR}/ImageProcessing
								${PROJECT_SOURCE_DIR}/OpenCV
								${PROJECT_SOURCE_DIR}/Stereo
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
	
set(${PROJECT_NAME}_LIBRARY_DIRS 
								${PROJECT_SOURCE_DIR}/Core
								${PROJECT_SOURCE_DIR}/Comms
								${PROJECT_SOURCE_DIR}/DataStructures
								${PROJECT_SOURCE_DIR}/Dialogs
								${PROJECT_SOURCE_DIR}/ImageProcessing
								${PROJECT_SOURCE_DIR}/OpenCV
								${PROJECT_SOURCE_DIR}/Stereo
    CACHE INTERNAL "${PROJECT_NAME}: Libraries Directories" FORCE)
	

add_library( ${PROJECT_NAME} STATIC ${Project_App_SRCS} )

target_link_libraries( ${PROJECT_NAME}
	AmariField2D
	StereoVidere
)

Link_libraries_to_project()

#-------------------------------------------------------------------------------
# Delay loading of some OpenCV modules to avoid getting wrong memory leak information
	# Delay_Load_OpenCV_Modules( 
		# core
		# highgui
		# imgproc
		# objdetect
	# )
	
#-------------------------------------------------------------------------------
# Configure project dependencies of other projects and respective libraries
set( PROJECT_DEPENDENCIES ${EXTERNAL_PROJECTS_LIST} ${USED_OPTIONAL_PROJECTS} )

foreach( dep ${PROJECT_DEPENDENCIES} )
	add_dependencies( ${PROJECT_NAME} ${dep} )
endforeach()

#-------------------------------------------------------------------------------
# Generate an include file that includes all headers of ProcessImage at once
Generate_Header_File_Aggregator( ${CMAKE_CURRENT_SOURCE_DIR}/ImageProcessing AllProcessImage.h ) 
