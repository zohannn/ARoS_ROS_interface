# ARoS ROS Interface
This is a MFC program to inteface the humanoid robot [ARoS](https://github.com/zohannn/aros_description) with [ROS](http://wiki.ros.org/hydro) (compatibility with Windows XP, MS Visual Studio 2010 and ROS Hydro)

## Installation

1. Install ROS Hydro on Windows XP: [https://gianpaoloslab.blogspot.com/2017/06/installation-of-ros-hydro-on-windows-xp.html](https://gianpaoloslab.blogspot.com/2017/06/installation-of-ros-hydro-on-windows-xp.html) 

2. Download the [ACE wrappers](https://mega.nz/file/JCQF0KgA#qObQqMbwSJNgbdiZ93bk66CORwN1snCj281DRCuVss0) into the C:\local\ACE_wrappers folder, define the environment variables ACE_ROOT = C:\local\ACE_wrappers and ACE_LIB = %ACE_ROOT%\lib and put them in the PATH variable.
	
3. Download and install [YARP](https://mega.nz/file/YDoTCSbQ#F2HQ_iqX0SaChKvwFkGjetiOf99jBg_aHSyzhUKbAFg):

	1. CMake the program in a build directory with the following options:	
	
		- uncheck CREATE_YMANAGER
		
		- uncheck YARP_COMPILE_TESTS 
		
		- set CMAKE_INSTALL_PREFIX = C:\local\YARP
		
	2. Compile and install YARP with visual studio 2010.
	3. Define the environment variable YARP_ROOT = C:\local\YARP and put %YARP_ROOT%\bin %YARP_ROOT%\include and %YARP_ROOT%\lib in the PATH variable.
	
	
4. Download the [serial port drivers](https://mega.nz/file/cColSKxI#bExDoFvs8CU4ZLX0jI1jW-uchnmkg9lZvdanm64PZq4) under C:\serial_asio folder. 
	Define the environment variableS SERIAL_ASIO_ROOT=C:\serial_asio\install, SERIAL_ASIO_INCLUDE=%SERIAL_ASIO_ROOT%\include, SERIAL_ASIO_LIB=%SERIAL_ASIO_ROOT%\lib and put them in the PATH variable. 
	
5. Download the [stereo vision drivers](https://mega.nz/file/MfxR0ApQ#mpIefkK0pOwlzuM8s6ZcI4tAnNiLzPK9NnmNa7-Ek-E) under the C:\SVS44 folder.
	Define the environment variables SVS_ROOT=C:/SVS44 and put it in the PATH variable.	
	
6. Download the [amtec arm drivers](https://mega.nz/file/UDxFxCBA#t8fpfAAVXjZgwVcaHMsbn18srr2rjCRUFXsGWiYyK6g) under the C:\amtec_lwa_7dof folder.
	Define the environment variableS AMTEC_LWA_7DOF_ROOT=C:\amtec_lwa_7dof, AMTEC_LWA_7DOF_INCLUDE=%AMTEC_LWA_7DOF_ROOT%\include, AMTEC_LWA_7DOF_LIB=%AMTEC_LWA_7DOF_ROOT%\lib and put them in the PATH variable. 
	
7. Download the [pantilt drivers](https://mega.nz/file/sLZS2ZYI#ufL0qi6C_D0Lz1ImMqKRepgnVMVvZrC4qyVhuwMU3s0) under the C:\pantilt_ptud46 folder.
	Define the environment variableS PANTILT_PTUD46_ROOT=C:\pantilt_ptud46\install, PANTILT_PTUD46_INCLUDE=%PANTILT_PTUD46_ROOT%\include, PANTILT_PTUD46_LIB=%PANTILT_PTUD46_ROOT%\lib and put them in the PATH variable. 	
	
6. Download the [intel drivers](https://mega.nz/file/lSgiyCjb#TqJcQb-fDuHzZUnemy36doHhcvy-_CjcGR4c3g1PkFk) under the C:\local\Intel folder and set the environment variable IPP_ROOT = C:\local\Intel\IPP\6.1.2.041\ia32.
	
7. Download the [OpenCV libraries](https://mega.nz/file/0K5Uiaob#66FNPhh8N3yZtQO0TDi-ckkue330SBO2zposDnSvCtw) under the C:\local\opencv folder, set the environmental variables OPENCV_ROOT=C:\local\opencv, OPENCV_BIN=%OPENCV_ROOT%\bin, OPENCV_LIB = %OPENCV_ROOT%\lib, OPENCV_INCLUDE = %OPENCV_ROOT%\include and put them in the PATH variable.
		
8. Clone the [dependencies](https://github.com/zohannn/ARoS_ROS_interface_DEPS) under the C:\ARoS_ROS_interface_DEPS folder and define the environment variable MAINPROJECT = C:\ARoS_ROS_interface_DEPS.

9. Compile in Release mode.
	
	See [how to Debug in Release mode?](https://stackoverflow.com/questions/11253334/how-to-debug-in-release-mode)
	
