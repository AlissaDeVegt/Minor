#pragma once


#ifdef CARD_PLATFORM_WINDOWS
	#ifdef CARD_BUILD_DLL //anything that has this defined in settings will export otherwise CardApi will import
		#define CARD_API __declspec(dllexport)
	#else
		#define CARD_API __declspec(dllimport)
	#endif 
#else
	#error not windows
#endif 
