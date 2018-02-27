#ifndef _AMTEC_PSTEPSERVER_H_
#define _AMTEC_PSTEPSERVER_H_

#pragma once
class CAmtecPStepServer
{
public:
	CAmtecPStepServer( void );
	~CAmtecPStepServer( void );

	bool Initialize( void );
	bool Terminate( void );

protected:
	void
		* PStepServer_proc,
		* PStepServer_startup_info;
};

#endif // _AMTEC_PSTEPSERVER_H_
