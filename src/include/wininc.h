#ifndef _EMERALDDB_WININC_H
#define _EMERALDDB_WININC_H

#ifdef _WINDOWS

#include <winsock2.h>
#include <windows.h>
#include <limits.h>
#include <ws2tcpip.h>

#define PATH_MAX	512
#define __func__	__FUNCTION__
#define STDOUT_FILENO	0
#define MSG_NOSIGNAL	0
#define EWOULDBLOCK		WSAEWOULDBLOCK	

typedef __int64		ssize_t;
typedef __int32		socklen_t;

// function
#ifdef __cplusplus
extern "C" {
#endif

unsigned sleep(unsigned seconds);
unsigned getpid(void);
unsigned pthread_self();
struct tm *localtime_r(const time_t *timer, struct tm *result);

#define snprintf	sprintf_s

#endif // _WINDOWS

#ifdef __cplusplus
};
#endif

#endif	// _EMERALDDB_WININC_H