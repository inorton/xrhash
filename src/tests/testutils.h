#include <sys/time.h>
#ifndef XR_TESTUTILS_H
#define XR_TESTUTILS_H

#include <stdlib.h>

long long
timeval_diff(struct timeval *difference,
             struct timeval *end_time,
             struct timeval *start_time
            );

#endif

