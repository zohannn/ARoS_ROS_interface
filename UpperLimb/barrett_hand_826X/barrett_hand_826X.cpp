#include "barrett_hand_826X.h"

#include <boost\array.hpp>
#include <boost\regex.hpp>
#include <boost\asio.hpp>
#include <boost\lambda\lambda.hpp>
#include <boost\lexical_cast.hpp>

#include <sstream>

//------------------------------------------------------------------------------------
BarrettHand826X::BarrettHand826X( void ): strStop( "=> " )
{
	BHand_Finger_ID[ BarrettHand::Finger::Spread ] = 'S';
	BHand_Finger_ID[ BarrettHand::Finger::One ] = '1';
	BHand_Finger_ID[ BarrettHand::Finger::Two ] = '2';
	BHand_Finger_ID[ BarrettHand::Finger::Three ] = '3';

	BHand_Device_2_Module_Idx[0] = BarrettHand::Finger::One;
	BHand_Device_2_Module_Idx[1] = BarrettHand::Finger::Two;
	BHand_Device_2_Module_Idx[2] = BarrettHand::Finger::Three;
	BHand_Device_2_Module_Idx[3] = BarrettHand::Finger::Spread;

	BHand_SetParam_Ratio[ BarrettHand::Finger::Spread ] = BHAND_RATIO_3150_180;
	BHand_SetParam_Ratio[ BarrettHand::Finger::One ] = BHAND_RATIO_17800_140;
	BHand_SetParam_Ratio[ BarrettHand::Finger::Two ] = BHAND_RATIO_17800_140;
	BHand_SetParam_Ratio[ BarrettHand::Finger::Three ] = BHAND_RATIO_17800_140;

	BHand_GetParam_Ratio[ BarrettHand::Finger::Spread ] = BHAND_RATIO_180_3150;
	BHand_GetParam_Ratio[ BarrettHand::Finger::One ] = BHAND_RATIO_140_17800;
	BHand_GetParam_Ratio[ BarrettHand::Finger::Two ] = BHAND_RATIO_140_17800;
	BHand_GetParam_Ratio[ BarrettHand::Finger::Three ] = BHAND_RATIO_140_17800;
}

bool BarrettHand826X::open( std::string cCom, long lBaudRate, double dCTfingers, double dCTspread, double dOTfingers, double dOTspread, double dSpeedfingers, double dSpeedspread, double dAccelfingers, double dAccelspread, long lMaxStrainfingers, long lMaxStrainspread, long iInitialize)
{	
	/*
	yarp::os::Property prop;
	
	prop.put( "device", "serial_asio" );
	prop.put( "com", cCom.c_str() );
	prop.put( "baud", lBaudRate );
	prop.put( "databits", 8 );
	prop.put( "stopbits", 1 );
	prop.put( "parity", "none" );
	
	if( !serial_port.open( prop ) ) return false;
	*/
	if(!serial_port.open(cCom,lBaudRate,8,1,"none")) return false;
	
	if( iInitialize==1 )
	{
		if( !Initialize() ) return false;
	}
	//-----------------------------------------------------------------------------
	// Open target
	double refs[4] = { dOTspread, dOTfingers, dOTfingers, dOTfingers };

	if( !setOpenTarget( refs ) ) return false;
		
	//-----------------------------------------------------------------------------
	// Close target
	refs[0] = dCTspread;
	refs[1] = dCTfingers;
	refs[2] = dCTfingers;
	refs[3] = dCTfingers;
	
	if( !setCloseTarget( refs ) ) return false;
		
	//-----------------------------------------------------------------------------
	// Finger velocity
	refs[0] = dSpeedspread;
	refs[1] = dSpeedfingers;
	refs[2] = dSpeedfingers;
	refs[3] = dSpeedfingers;
	
	if( !setRefSpeeds( refs ) ) return false;
		
	//-----------------------------------------------------------------------------
	// Finger acceleration
	refs[0] = dAccelspread;
	refs[1] = dAccelfingers;
	refs[2] = dAccelfingers;
	refs[3] = dAccelfingers;
	
	if( !setRefAccelerations( refs ) ) return false;

	//-----------------------------------------------------------------------------
	// Finger strain
	int lrefs[4] = { lMaxStrainspread, lMaxStrainfingers, lMaxStrainfingers, lMaxStrainfingers };
	
	if( !setMaxStrain( lrefs ) ) return false;
	
	//-----------------------------------------------------------------------------
	//double refs_pos[4];

	//refs_pos[0]=0;
	//refs_pos[1]=45;
	//refs_pos[2]=45;
	//refs_pos[3]=45;

	return positionMoveFingers(45.0);

	//return OpenHand();
}


bool BarrettHand826X::close( void )
{
	return serial_port.close();
}

bool BarrettHand826X::getAxes( int *ax )
{
	*ax = getAxes();
	return true;
}

int BarrettHand826X::getAxes( void )
{
	return BarrettHand::Finger::BARRETT_HAND_NUMBER_OF_DOF;
}

bool BarrettHand826X::isValidAxis( int axis )
{
	return ( ( axis >= 0 ) && ( axis < getAxes() ) );
}

bool BarrettHand826X::Read_Reply( std::string& sReply, int timeout_ms )
{
	int nBytes = serial_port.Read_Wait_String( 
		sReply, 
		strStop, 
		timeout_ms );

	return ( nBytes > 0 );
}

bool BarrettHand826X::Read_Reply_Discard( int timeout_ms )
{
	std::string reply = "";
	return Read_Reply( reply, timeout_ms );
}

bool BarrettHand826X::Write_Command( std::string command )
{
	std::string cmd = command;
	cmd += "\r";
	return serial_port.Write( cmd );
}

bool BarrettHand826X::Write_Command_Discard_Reply( std::string command, int timeout_ms )
{
	if( !Write_Command( command ) ) return false;

	return Read_Reply_Discard( timeout_ms );
}

bool BarrettHand826X::Write_Command_Read_Reply( std::string command, std::string &sReply, int timeout_ms )
{
	if( !Write_Command( command ) ) return false;

	return Read_Reply( sReply, timeout_ms );
}

bool BarrettHand826X::Write_Command_Read_Reply_Clean( std::string command, std::string &sReply, int timeout_ms )
{
	bool bSuccess = Write_Command_Read_Reply( command, sReply, timeout_ms );
	
	if( bSuccess )
	{
		size_t start_pos = 0;

		// Remove initial command from the reply
		if( ( start_pos = sReply.find( command, start_pos ) ) != std::string::npos )
		{
			sReply.erase( start_pos, command.length() );
		}

		start_pos = 0;

		// Remove everything after the stop string from the reply
		if( ( start_pos = sReply.find( strStop, start_pos ) ) != std::string::npos )
		{
			sReply.erase( start_pos, sReply.size() - start_pos );//strStop.length() );
		}

		start_pos = 0;

		// Remove all the "\n\r" from the reply
		while( ( start_pos = sReply.find( "\n\r", start_pos ) ) != std::string::npos )
		{
			sReply.erase( start_pos, 2 );
			start_pos = 0;
		}
	}

	return bSuccess;
}

bool BarrettHand826X::Parse_int_from_command_reply( std::string reply, std::string command, int * val, int conv )
{
	int value = 0;

	std::stringstream strm_format;

	strm_format << command << "\n\r%d\n\r" << strStop;

	if( sscanf( reply.c_str(), strm_format.str().c_str(), &value ) > 0 )
	{
		*val = value * conv;
		return true;
	}
	else
	{
		return false;
	}
}

bool BarrettHand826X::Parse_double_from_command_reply( std::string reply, std::string command, double * val, double conv )
{
	int value = 0;

	if( Parse_int_from_command_reply( reply, command, &value ) )
	{
		*val = static_cast<double>( value ) * conv;
		return true;
	}
	else
	{
		return false;
	}
}

	
bool BarrettHand826X::getAxisName( int axis, std::string& name )
{
	if( !isValidAxis( axis ) ) return false;

	name = BarrettHand::Finger::Name[axis];

	return true;
}

bool BarrettHand826X::Initialize( int *vec )
{
	std::stringstream command( "" );

	int i=0;

	do
	{
		if( isValidAxis( i ) )
			command << BHand_Finger_ID[vec[i]];

		i++;
	}while( vec[i]!=-1 );

	command << "HI";

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::Initialize( void )
{
	return Write_Command_Discard_Reply( "HI", 20000 );
}

bool BarrettHand826X::OpenHand( void )
{
	return Write_Command_Discard_Reply( "GO" );
}

bool BarrettHand826X::CloseHand( void )
{
	return Write_Command_Discard_Reply( "GC" );
}

bool BarrettHand826X::OpenHand( int *vec )
{
	std::stringstream command( "" );

	int i=0;

	do
	{
		if( isValidAxis( i ) )
			command << BHand_Finger_ID[vec[i]];

		i++;
	}while( vec[i]!=-1 );

	command << "O";

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::CloseHand( int *vec )
{
	std::stringstream command( "" );

	int i=0;
	do
	{
		if( isValidAxis( i ) )
			command << BHand_Finger_ID[vec[i]];

		i++;
	}
	while( vec[i]!=-1 );
	
	command << "C";

	return Write_Command_Discard_Reply( command.str() );
}	

bool BarrettHand826X::TorqueOpen( void )
{
	return Write_Command_Discard_Reply( "GTO" );
}

bool BarrettHand826X::TorqueClose( void )
{
	return Write_Command_Discard_Reply( "GTC" );
}

bool BarrettHand826X::TorqueOpen( int *vec )
{
	std::stringstream command( "" );

	int i=0;
	
	do
	{
		if( isValidAxis( i ) )
			command << BHand_Finger_ID[vec[i]];

		i++;
	}
	while( vec[i]!=-1 );
	
	command << "TO";

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::TorqueClose( int *vec )
{
	std::stringstream command( "" );

	int i=0;

	do
	{
		if( isValidAxis( i ) )
			command << BHand_Finger_ID[vec[i]];

		i++;
	}
	while( vec[i]!=-1 );
	
	command << "TC";

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::setOpenTarget( double *refs )
{
	for( int i=0 ; i<4 ; i++ )
	{
		if( !setOpenTarget( i, refs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::setOpenTarget( int j, double ref )
{
	if( !isValidAxis( j ) ) return false;

	int value = static_cast<int>( ref * BHand_SetParam_Ratio[j] );

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FSET OT " << value;

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::setCloseTarget( double *refs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !setCloseTarget( i, refs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::setCloseTarget( int j, double ref )
{
	if( !isValidAxis( j ) ) return false;

	int value = static_cast<int>( ref * BHand_SetParam_Ratio[j] );

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FSET CT " << value;
		
	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::getOpenTarget( double *refs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !getOpenTarget( i, &refs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::getOpenTarget( int j, double *ref )
{
	 if( !isValidAxis( j ) ) return false;

	std::string msgout = "";
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET OT";
	
	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;
	
	return Parse_double_from_command_reply( msgout, command.str(), ref, BHand_GetParam_Ratio[j] );
}

bool BarrettHand826X::getCloseTarget( double *refs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !getCloseTarget( i, &refs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::getCloseTarget( int j, double *ref )
{
	 if( !isValidAxis( j ) ) return false;

	std::string msgout = "";

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET CT";

	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;
	
	return Parse_double_from_command_reply( msgout, command.str(), ref, BHand_GetParam_Ratio[j] );
}

bool BarrettHand826X::positionMoveFingersRaw( double ref )
{
	std::stringstream command;
	command << "123M " << static_cast<int>( ref );

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::positionMoveFingers( double ref )
{
	return positionMoveFingersRaw( ref*BHAND_RATIO_17800_140 );
}

bool BarrettHand826X::positionMoveFingers( const double *refs )
{	
	for( int i=1 ; i<getAxes() ; i++ )
	{
		if( !positionMove( i, refs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::positionMoveTwoFingersRaw( double *refs )
{
	std::stringstream command( "" );
	command << "12M " << static_cast<int>( refs[ BarrettHand::Finger::One ] );
	
	if( !Write_Command_Discard_Reply( command.str() ) ) return false;

	command.str( "" );
	command.clear();

	command << "3M " << static_cast<int>( refs[ BarrettHand::Finger::Three ] );

	return Write_Command_Discard_Reply( command.str() ); 
}

bool BarrettHand826X::positionMoveTwoFingers( double *refs )
{
	refs[ BarrettHand::Finger::One ] *= BHAND_RATIO_17800_140;
	refs[ BarrettHand::Finger::Three ] *= BHAND_RATIO_17800_140;

	return positionMoveTwoFingersRaw( refs );
}

bool BarrettHand826X::setRefSpeedOpen( int j, double sp )
{
	 if( !isValidAxis( j ) ) return false;

	const double 
		dFSpeed_Max[4] = { 60.0, 100.0, 100.0, 100.0 },
		dFSpeed_Min[4] = { 16.0, 16.0, 16.0, 16.0 };

	if( sp>dFSpeed_Max[j] )
		sp = dFSpeed_Max[j];

	if( sp<dFSpeed_Min[j] )
		sp = dFSpeed_Min[j];

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FSET MOV " << static_cast<int>( sp );
		
	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::setRefSpeedsOpen( const double *spds )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !setRefSpeedOpen(i, spds[i]) )
			return false;
	}
	return true;
}

bool BarrettHand826X::setRefSpeedClose( int j, double sp )
{
	 if( !isValidAxis( j ) ) return false;

	const double 
		dFSpeed_Max[4] = { 60.0, 100.0, 100.0, 100.0 },
		dFSpeed_Min[4] = { 16.0, 16.0, 16.0, 16.0 };

	if( sp>dFSpeed_Max[j] )
		sp = dFSpeed_Max[j];

	if( sp<dFSpeed_Min[j] )
		sp = dFSpeed_Min[j];

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FSET MCV " << static_cast<int>( sp );
	
	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::setRefSpeedsClose( const double *spds )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !setRefSpeedClose( i, spds[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::getPositionsRaw( double *pos )
{
	bool value = true;

	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !getPositionRaw( i, &pos[i] ) )
			value = false;
	}
		
	return value;
}

bool BarrettHand826X::getPositionRaw( int j, double *ref )
{
	if( !isValidAxis( j ) ) return false;

	std::string msgout = "";

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET P";

	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;

	return Parse_double_from_command_reply( msgout, command.str(), ref );
}

bool BarrettHand826X::getPositions( double *pos )
{
	using boost::lambda::var;
	using boost::lambda::_1;
	using boost::lambda::_2;
	try
	{
		boost::shared_ptr<boost::asio::serial_port> serial_dev;
		boost::asio::streambuf buf;
		boost::asio::streambuf in_buf;

		size_t bytes_transfered = 0;
		boost::system::error_code ec = boost::asio::error::would_block;

		//get the low level device.
		serial_dev = serial_port.GetSerialPortDevice();

		//update current positions
		buf.sputn("FGET P\r", 7);
		boost::asio::write(*serial_dev, buf);
		boost::regex fgetpos_pattern(".*FGET P\\n\\r(\\d+) (\\d+) (\\d+) (\\d+)\\n\\r=> |.*ERR (\\d+)\\n\\r=> ");

	//	serial_port.dead_line_timer->async_wait( boost::bind( &yarp::dev::serial_asio::checkdeadline, boost::ref(serial_port )) );
		serial_port.dead_line_timer->expires_from_now( boost::posix_time::milliseconds( 1000 ) );
		
		 // Start the asynchronous operation itself. The boost::lambda function
		// object is used as a callback and will update the ec variable when the
		// operation completes. The blocking_udp_client.cpp example shows how you
		// can use boost::bind rather than boost::lambda.
//		boost::asio::async_read_until(*serial_dev, in_buf, fgetpos_pattern, (var(ec) = _1, var(bytes_transfered) = _2) );
		
		 // Block until the asynchronous operation has completed.
	//	do serial_dev->get_io_service().run_one(); while (ec == boost::asio::error::would_block);

//		if (ec)
//			throw boost::system::system_error(ec);

		serial_port.io_service.post(
		[this, &serial_dev, &ec, &bytes_transfered, &in_buf,&fgetpos_pattern]
		{
			boost::asio::async_read_until( *serial_dev, in_buf, fgetpos_pattern, (boost::lambda::var(ec) = _1, boost::lambda::var(bytes_transfered) = _2) );
			 // Block until the asynchronous operation has completed.
		});
		do ; while (ec == boost::asio::error::would_block);


	//	bytes_transfered = boost::asio::read_until(*serial_dev, in_buf, fgetpos_pattern, ec);

		if (ec)
			throw boost::system::system_error(ec);

		serial_port.dead_line_timer->expires_at( boost::posix_time::pos_infin );

		std::string str;
		str.resize(bytes_transfered);
		in_buf.sgetn(&str.front(), bytes_transfered);

		boost::smatch matches;

		if (boost::regex_match(str, matches, fgetpos_pattern))
		{
			if(matches.size() < 4)
				std::cout<<"barret error"<<std::endl;
			else
			for (size_t i = 1; i < matches.size() - 1; i++)
				pos[i - 1] = boost::lexical_cast<double>(std::string(matches[i].first, matches[i].second)) * BHand_GetParam_Ratio[BHand_Device_2_Module_Idx[i - 1]];
		}
		else
			return false;
	}
	catch(boost::system::system_error &ex)
	{
		if(ex.code() == boost::asio::error::operation_aborted)
			std::cerr << "Bhand: Timeout reading from serial port: " << ex.what() << std::endl;
		else
			std::cerr << "Bhand: " << ex.what() << std::endl;
		return false;
	}
	catch (boost::bad_lexical_cast const& ex)
	{
		std::cerr << "Could not convert hand current positions: " << ex.what() << std::endl;
		return false;
	}
	catch (boost::regex_error const& ex)
	{
		std::cerr << "Regex Error: " << ex.what() << std::endl;
		return false;
	}
	//swap to   S F1 F2 F3
		// from F1 F2 F3 S
	double temp;
	temp = pos[3];
	pos[3] = pos[2];
	pos[2] = pos[1];
	pos[1] = pos[0];
	pos[0] = temp;
	
	return true;
}

bool BarrettHand826X::getPosition( int j, double *ref )
{
	double val = 0.0;

	if( getPositionRaw( j, &val ) )
	{
		*ref = val * BHand_GetParam_Ratio[j];
		return true;
	}
	else
	{
		return false;
	}
}


bool BarrettHand826X::getStrainGauge( int j, double *strain )
{
	 if( !isValidAxis( j ) ) return false;

	std::string msgout = "";
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET SG";

	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;
	
	return Parse_double_from_command_reply( msgout, command.str(), strain );
}

bool BarrettHand826X::getStrainGauges( double *strain )
{
	for( int i=0; i<getAxes() ; i++ )
	{
		if( !getStrainGauge( i, &strain[i] ) )
			return false;
	}
	return true;
}

// ########################################################################

bool BarrettHand826X::setPositionModeRaw( void )
{
	return false;
}

bool BarrettHand826X::setPositionMode( void )
{
	// No conversion required
	return setPositionModeRaw();
}

bool BarrettHand826X::positionMoveRaw( int j, double ref )
{
	 if( !isValidAxis( j ) ) return false;

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "M " << static_cast<int>( ref );

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::positionMove( int j, double ref )
{
	return 
		isValidAxis( j ) ?
		positionMoveRaw( j, ref * BHand_SetParam_Ratio[j] ) :
		false;
}

bool BarrettHand826X::positionMoveRaw( const double *refs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !positionMoveRaw( i, refs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::positionMove( const double *refs )
{	
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !positionMove( i, refs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::relativeMoveRaw( int j, double delta )
{
	 if( !isValidAxis( j ) ) return false;

	char cOption = ( delta>0.0 ) ? 'C' : 'O';
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "I" << cOption << " " << abs( static_cast<int>( delta ) );

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::relativeMove( int j, double delta )
{
	return 
		isValidAxis( j ) ?
		relativeMoveRaw( j, delta * BHand_SetParam_Ratio[j] ) :
		false;
}

bool BarrettHand826X::relativeMoveRaw( const double *deltas )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !relativeMoveRaw( i, deltas[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::relativeMove( const double *deltas )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !relativeMove( i, deltas[i] ) )
			return false;
	}
		
	return true;
}

bool BarrettHand826X::checkMotionDoneRaw( int j, bool *flag )
{
	int status = 0;
	bool bSuccess = getStatus( j, &status );
	*flag = ( status == 0 );
	return bSuccess;
}

bool BarrettHand826X::checkMotionDone( int j, bool *flag )
{
	// No converion required
	 return checkMotionDoneRaw( j, flag );
}

bool BarrettHand826X::checkMotionDoneRaw( bool *flag )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !checkMotionDoneRaw( i, flag ) )
		{
			*flag = false;
			return false;
		}
	}
	return true;
}

bool BarrettHand826X::checkMotionDone( bool *flag )
{
	// No converion required
	return checkMotionDoneRaw( flag );
}

bool BarrettHand826X::setRefSpeedRaw( int j, double sp )
{
	 if( !isValidAxis( j ) ) return false;

	const double 
		dFSpeed_Max[4] = { 60.0, 100.0, 100.0, 100.0 },
		dFSpeed_Min[4] = { 16.0, 16.0, 16.0, 16.0 };

	
	if( sp>dFSpeed_Max[j] )
		sp = dFSpeed_Max[j];

	if( sp<dFSpeed_Min[j] )
		sp = dFSpeed_Min[j];

	int ispeed = static_cast<int>( sp );
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FSET MCV " << ispeed;

	if( !Write_Command_Discard_Reply( command.str() ) ) return false;

	command.str( "" );
	command.clear();

	command << BHand_Finger_ID[j] << "FSET MOV " << ispeed;

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::setRefSpeed( int j, double sp )
{
	// No converion required
	return setRefSpeedRaw( j, sp );
}

bool BarrettHand826X::setRefSpeedsRaw( const double *spds )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !setRefSpeedRaw( i, spds[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::setRefSpeeds( const double *spds )
{
	// No converion required
	return setRefSpeedsRaw( spds );
}

bool BarrettHand826X::setRefAccelerationRaw( int j, double acc )
{
	if( !isValidAxis( j ) ) return false;

	double dAcc = acc;
	
	if( dAcc>10.0 ) dAcc = 10.0;
	 
	if( dAcc<1.0 ) dAcc = 1.0;
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FSET ACCEL " << static_cast<int>( dAcc );

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::setRefAcceleration( int j, double acc )
{
	// No conversion required
	return setRefAccelerationRaw( j, acc );
}

bool BarrettHand826X::setRefAccelerationsRaw( const double *accs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !setRefAccelerationRaw( i, accs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::setRefAccelerations( const double *accs )
{
	// No conversion required
	return setRefAccelerationsRaw( accs );
}


bool BarrettHand826X::setMaxStrain( int * refs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !setMaxStrain( i, refs[i] ) )
			return false;
	}
	return true;
}
	

bool BarrettHand826X::setMaxStrain( int j, int ref )
{
	if( !isValidAxis( j ) ) return false;

	if( ref>256 ) ref = 256;
	 
	if( ref<0 ) ref = 0;
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FSET HSG " << ref;

	return Write_Command_Discard_Reply( command.str() );
}

bool BarrettHand826X::getMaxStrain( int * refs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !getMaxStrain( i, &refs[i] ) )
			return false;
	}
	return true;
}
	
bool BarrettHand826X::getMaxStrain( int j, int * ref )
{
	 if( !isValidAxis( j ) ) return false;

	std::string msgout = "";
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET HSG";

	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;
	
	return Parse_int_from_command_reply( msgout, command.str(), ref );
}

bool BarrettHand826X::getRefSpeedRaw( int j, double *ref )
{
	 if( !isValidAxis( j ) ) return false;

	std::string msgout = "";
	
	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET MOV";

	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;
	
	return Parse_double_from_command_reply( msgout, command.str(), ref );
}

bool BarrettHand826X::getRefSpeed( int j, double *ref )
{
	// No conversion required
	return getRefSpeedRaw( j, ref );
}

bool BarrettHand826X::getRefSpeedsRaw( double *spds )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !getRefSpeedRaw( i, &spds[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::getRefSpeeds( double *spds )
{
	// No conversion required
	return getRefSpeedsRaw( spds );
}

bool BarrettHand826X::getRefAccelerationRaw( int j, double *acc )
{
	 if( !isValidAxis( j ) ) return false;

	std::string msgout = "";

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET ACCEL";

	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;
	
	return Parse_double_from_command_reply( msgout, command.str(), acc );
}

bool BarrettHand826X::getRefAcceleration( int j, double *acc )
{
	// No conversion required
	return getRefAccelerationRaw( j, acc );
}

bool BarrettHand826X::getRefAccelerationsRaw( double *accs )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !getRefAccelerationRaw( i, &accs[i] ) )
			return false;
	}
	return true;
}

bool BarrettHand826X::getRefAccelerations( double *accs )
{
	// No conversion required
	return getRefAccelerationsRaw( accs );
}

bool BarrettHand826X::stopRaw( int j )
{
	return false;
}

bool BarrettHand826X::stop( int j )
{
	return stopRaw( j );
}

bool BarrettHand826X::stopRaw( void )
{
	return false;
}

bool BarrettHand826X::stop( void )
{
	return stopRaw();
}

bool BarrettHand826X::getStatus( int j, int * status )
{
	 if( !isValidAxis( j ) ) return false;

	std::string msgout = "";

	std::stringstream command( "" );
	command << BHand_Finger_ID[j] << "FGET S";

	if( !Write_Command_Read_Reply( command.str(), msgout ) ) return false;
	
	int value = 0;

	if( Parse_int_from_command_reply( msgout, command.str(), &value ) )
	{
		*status = value;
		return true;
	}
	else
	{
		return false;
	}
}

bool BarrettHand826X::getStatus( int * status )
{
	for( int i=0 ; i<getAxes() ; i++ )
	{
		if( !getStatus( i, &status[i] ) )
			return false;
	}
	
	return true;
}

bool BarrettHand826X::getStatus( int j, int * status, std::string &str_status )
{
	if( !getStatus( j, status ) ) return false;

	return getStatus_str( *status, str_status );
}

bool BarrettHand826X::getStatus_str( int status, std::string &str_status )
{
	bool bSuccess = true;

	switch( status )
	{
	case 0:
		str_status = "No error";
		break;

	case 1:
		str_status = "No motor board found";
		break;

	case 2:
		str_status = "No motor found";
		break;

	case 4:
		str_status = "Motor not initialized";
		break;

	case 8:
	case 512:
		str_status = "(not used)";
		break;

	case 16:
		str_status = "Couldn't reach position";
		break;

	case 32:
		str_status = "Unknown command";
		break;

	case 64:
		str_status = "Unknown parameter name";
		break;

	case 128:
		str_status = "Invalid value";
		break;

	case 256:
		str_status = "Tried to write a read only parameter";
		break;

	case 1024:
		str_status = "Too many arguments for this command";
		break;

	case 2048:
		str_status = "Invalid RealTime control block header";
		break;

	case 4096:
		str_status = "Command can't have motor prefix";
		break;

	case 8192:
		str_status = "Overtemperature fault tripped";
		break;

	case 16384:
		str_status = "Cntl-C abort command received";
		break;

	default:
		str_status = "Invalid status code";
		bSuccess = false;
		break;
	}

	return bSuccess;
}