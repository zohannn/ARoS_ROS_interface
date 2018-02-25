#pragma once


#include <ros\ros.h>
#include <ros\network.h> 

// Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

// CNode
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;


using namespace logging::trivial;

class CNode : public CWinThread
{
	DECLARE_DYNCREATE(CNode)

//protected:
public:
	CNode();           
	virtual ~CNode();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	bool on_init();
	bool on_init(CString master,CString ip);
	bool on_end();
	int Run();
	CString getNodeName();
	void setNodeName(CString name);

	
	/**
    * @brief This enumerator is used for logging functionalities
    */
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

    /**
        * @brief This method return the list of loggings
        * @return
        */
    //QStringListModel* loggingModel() { return &logging_model; }

	/**
    * @brief This method runs logging of the passed message
    * @param level
    * @param msg
    */
     //void log( const LogLevel &level, const std::string &msg);

protected:
	const char * WinGetEnv(const char * name);
	DECLARE_MESSAGE_MAP()

private:
	CString node_name;
	void init_log();

};


