#ifdef _WINDOWS

#include "core.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
	#pragma comment(lib, "ws2_32.lib")
#else
	#error("do not the appropriate library");
#endif

const int SEC_PER_MSEC = 1000;

unsigned sleep(unsigned seconds)
{
	Sleep(seconds * SEC_PER_MSEC);
	return 0;
}

unsigned getpid(void)
{
	return (unsigned)GetCurrentProcessId();
}

unsigned pthread_self()
{
	return (unsigned)GetCurrentThreadId();
}

struct tm *localtime_r(const time_t *timer, struct tm *result)
{
	if (0 == localtime_s(result, timer) )
	{
		return result;
	}
	return NULL;
}

// initialize socket environment and release it
class __CSocketEnvironment
{
public:
	__CSocketEnvironment()
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
	}

	~__CSocketEnvironment()
	{
		WSACleanup();
	}
};

static __CSocketEnvironment		sgl_SocketEnvironment;

#ifdef __cplusplus
};
#endif

#endif // _WINDOWS