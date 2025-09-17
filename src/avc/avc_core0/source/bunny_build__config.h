#include "bunny_debug.h"

#ifndef BUNNY_BUILD__CONFIG_H_
#define BUNNY_BUILD__CONFIG_H_


#define BUNNY_BUILD_PRINTF(...)			     BUNNY_DBG(__VA_ARGS__)
#define BUNNY_BUILD_MAX_LABELS				 32

/*
	Output formatting for printf messages
*/


#define BUNNY_BUILD_ERROR_FLAG		E_VT100__RED"[ERROR] "E_VT100__DEFAULT
#define BUNNY_BUILD_WARNING_FLAG	E_VT100__YELLOW"[WARNING] "E_VT100__DEFAULT
#define BUNNY_BUILD_INFO_FLAG		E_VT100__GREEN"[INFO] "E_VT100__DEFAULT



#endif /* BUNNY_BUILD__CONFIG_H_ */
