#ifndef LINUXSPEC_H_
#define LINUXSPEC_H_

#include <string>
#include "time.h"

std::string get_basepath();

int stricmp( const char* c1, const char* c2 );

int WSAGetLastError();

#endif /*LINUXSPEC_H_*/
