#ifndef _COMMUNICATION_SERVER_PANTILT_H_
#define _COMMUNICATION_SERVER_PANTILT_H_

#include <pantilt_ptud46.h>


class CCommunicationServerPanTilt
{
private:
	std::string sConfig;
	
	//yarp::os::Property PanTilt_Properties;

	double 
		dVal,
		dVec[3];

public:
	PanTilt_PTUD46 PanTilt;

public:
	CCommunicationServerPanTilt( const std::string name, const std::string dev_config_file );
	~CCommunicationServerPanTilt( void );

	bool Init( void );
	void Fini( void );
	
	//void Process( CMessage &msgIn, CMessage &msgOut, void *private_data = nullptr );

protected:
	void ConvertCoordinatestoPanTilt( float fCoord[3], float *fPanTilt );

	void Print_Wrapper_Error( int error_code );

	void Print_Wrapper_Error( int error_code, int command );

	void Print_Wrapper_Error( int error_code, std::string strExtra );

	std::string CommandToString( unsigned int uCommand );

	//inline void Prepare_ReplyMessage_2Ax( CMessage * msgOut );
};

#endif //_COMMUNICATION_SERVER_PANTILT_H_
