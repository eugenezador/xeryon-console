#ifndef _XERYON_H
#define _XERYON_H

#if defined (_WIN32) || defined (_WIN64)
#include <windows.h>
#endif

#include "Distance.h"
#include "Axis.h"
#include "Stage.h"

#include <list>
#include <thread>
#include <future>

//#include <libexplain/gcc_attributes.h>
//#include <libexplain/large_file_support.h>
#include <unistd.h>
#include <errno.h>

class Axis;
class Stage;

class Xeryon
{
public:
	Xeryon(const char * port, int baudrate) : port_(port), baudrate_(baudrate) { }
	Axis * addAxis(const char letter, const Stage * stage);
	Axis * getAxis(const char letter);
	
	bool isSingleAxisSystem() { return axis_list_.size() <= 1; }
	
	void start();
	void stop();
	void reset();

	void stopMovements();

	void readSettings();
	
	void sendCommand(Axis * axis, const char * command);
private:
    int readPort_(char * c, unsigned int timeout);
	void processData_();
	std::thread comm_thread_;
	std::promise<void> comm_exit_;
	const char * port_;
	int baudrate_;
#if defined (_WIN32) || defined (_WIN64)
	HANDLE port_handle_;
	COMMTIMEOUTS timeouts_;
#endif
#if defined (__linux__) || defined(__APPLE__)
	int port_fd_;
#endif
	std::list<Axis *> axis_list_;
};
#endif
