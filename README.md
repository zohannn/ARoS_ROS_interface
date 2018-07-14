# ARoS_ROS_interface
This is a MFC program for ROS interface for the humanoid robot ARoS (compatibility with Windows XP and MS Visual Studio 2010)

Installation steps:

	1. Compile and install ROS: http://gianpaologulletta.blogspot.pt/2017/06/installation-of-ros-hydro-on-windows-xp.html
	
	2. Extract the ACE wrappers (https://mega.nz/#!wSoFxaCA!cpuLKVLSxyzDJSDtcCF2ZOfETHAJbn-tHniDmJA0Wio) into C:\local\ACE_wrappers.
	Define the environment variables ACE_ROOT = C:\local\ACE_wrappers and ACE_LIB = %ACE_ROOT%\lib. Put them in the PATH.
	
	3. Compile and install YARP (https://mega.nz/#!dfZESIIB!sgKvIa3w7rOGO3quw-NvBdpl2txHDZrxBigxkEejRkM). CMake the program with the following options:
		a. uncheck CREATE_YMANAGER
		b. uncheck YARP_COMPILE_TESTS 
		c. set CMAKE_INSTALL_PREFIX = C:\local\YARP
	Define the environment variable YARP_ROOT = C:\local\YARP and put %YARP_ROOT%\bin %YARP_ROOT%\include and %YARP_ROOT%\lib in the  PATH
	Compile and install YARP with visual studio 2010.
	
	2. Download the serial port drivers (https://mega.nz/#!kS4iHTSC!FMfBAI2wxRQZj8LiDA2L6SfS7yTwFhPARnennLDiCnQ) under C:\serial_asio. 
	Define the environment variableS SERIAL_ASIO_ROOT=C:\serial_asio\install, SERIAL_ASIO_INCLUDE=%SERIAL_ASIO_ROOT%\include and SERIAL_ASIO_LIB=%SERIAL_ASIO_ROOT%\lib. Put them in the path. 
	
	3. Download the stereo vision drivers (https://mega.nz/#!QSwVhT7Q!9kSTWxG4t9LxnZVy1nfJwgpVeilQ08olBzYeqzBhJmc) under C:\SVS44.
	Define the environment variables SVS_ROOT=C:/SVS44 and put it in the path.	
	
	4. Download the amtec arm drivers (https://mega.nz/#!tL4jmbSQ!7-hTHxfd9XbwhIQouJTHFghB5ZCitIadYYv9KgWZE2s) under C:\amtec_lwa_7dof.
	Define the environment variableS AMTEC_LWA_7DOF_ROOT=C:\amtec_lwa_7dof, AMTEC_LWA_7DOF_INCLUDE=%AMTEC_LWA_7DOF_ROOT%\include and AMTEC_LWA_7DOF_LIB=%AMTEC_LWA_7DOF_ROOT%\lib. Put them in the path. 
	
	5. Download the pantilt drivers (https://mega.nz/#!dPgnxLBZ!oKazvrYDtuKk49chPH0LXacEBJIP0LSxsQS1w3z_2SM) under C:\pantilt_ptud46.
	Define the environment variableS PANTILT_PTUD46_ROOT=C:\pantilt_ptud46\install, PANTILT_PTUD46_INCLUDE=%PANTILT_PTUD46_ROOT%\include and PANTILT_PTUD46_LIB=%PANTILT_PTUD46_ROOT%\lib. Put them in the path. 	
	
	6. Extract the intel IPP (https://mega.nz/#!IfZCwDqZ!JXKSSIR8HUCLhWTb2e4P8NPYz-floaWpZL85-8nJKlI) under C:\local\Intel and set the environment variable IPP_ROOT = C:\local\Intel\IPP\6.1.2.041\ia32.
	
	7. Extract the OpenCV libraries (https://mega.nz/#!lXoz3RQT!-iyRqhZGrvqJnUrlz__ef2dUOaBCwl9UuhNctIGD9-E) under C:\local\opencv and set the environmental variables OPENCV_ROOT=C:\local\opencv, OPENCV_BIN=%OPENCV_ROOT%\bin, 
		OPENCV_LIB = %OPENCV_ROOT%\lib, OPENCV_INCLUDE = %OPENCV_ROOT%\include. Put them in the PATH.
		
	8. Clone the dependencies (https://github.com/zohannn/ARoS_ROS_interface_DEPS) under C:/ARoS_ROS_interface_DEPS and define the environment variable MAINPROJECT = C:/ARoS_ROS_interface_DEPS.