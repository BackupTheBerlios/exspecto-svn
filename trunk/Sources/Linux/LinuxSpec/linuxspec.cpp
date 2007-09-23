#include "Os_Spec.h"
#include <errno.h>


std::string get_basepath()
{
	std::string path = "";
	pid_t pid = getpid();
	char buf[10];
	sprintf(buf,"%d",pid);
	std::string _link = "/proc/";
	_link.append( buf );
	_link.append( "/exe");
	char proc[512];
	int ch = readlink(_link.c_str(),proc,512);
	if (ch != -1)
	{
		proc[ch] = 0;
		path = proc;
		std::string::size_type t = path.find_last_of("/");
		path = path.substr(0,t);
	}
	return path;
}

int stricmp( const char* c1, const char* c2 )
{
    return strcasecmp( c1, c2 );
}

int WSAGetLastError()
{
    return errno;
}
