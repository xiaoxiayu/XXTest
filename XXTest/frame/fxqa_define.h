#ifndef _FXQA_DEFINE_H_
#define _FXQA_DEFINE_H_

#define QARUN(_interface) QA_DLOG(INFO)<<#_interface;_interface;

#if defined(_MSC_VER)
#include <Windows.h>
typedef CRITICAL_SECTION	FXQA_LOCK;
#elif defined(__GNUC__)
#include <pthread.h>
typedef pthread_mutex_t		FXQA_LOCK;
#else
typedef void*				FXQA_LOCK;
#endif

#endif
