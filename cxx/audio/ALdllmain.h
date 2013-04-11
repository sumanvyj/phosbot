#ifndef AL_SUBSYSTEM_H
#define AL_SUBSYSTEM_H

#include <list>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <stdint.h>
#include <fstream>

typedef float Real;
typedef std::string String;
typedef unsigned char byte;
typedef unsigned uint;

#include "SharedPtr.h"
#include "Exception.h"
#include "SoundData.h"

#ifdef WIN32

	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>

	#ifdef ALSubsystem_EXPORTS
		#define ORYX_AL_EXPORT __declspec(dllexport)
	#else
		#define ORYX_AL_EXPORT __declspec(dllimport)
	#endif

#else
	#define ORYX_AL_EXPORT
#endif

#endif
