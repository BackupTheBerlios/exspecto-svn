#ifndef PRECOMP_H_
#define PRECOMP_H_

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "windows.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <list>
#include <iostream>

#ifdef WIN32
#include <tchar.h>
#include <process.h>
#endif

#include "MemLeakDetector.h"
#include "CLog.h"
#include "SettingsContainer.h"




#endif /*PRECOMP_H_*/
