#include "common.h"

#ifndef HAVE_LLRINT
av_always_inline long long llrint(double x)
{
    return rint(x);
}
#endif /* HAVE_LLRINT */

#ifndef HAVE_LRINT
av_always_inline long int lrint(double x)
{
    return rint(x);
}
#endif /* HAVE_LRINT */

#ifndef HAVE_LRINTF
av_always_inline long int lrintf(float x)
{
    return (int)(rint(x));
}
#endif /* HAVE_LRINTF */

#ifndef HAVE_ROUND
av_always_inline double round(double x)
{
    return (x > 0) ? floor(x + 0.5) : ceil(x - 0.5);
}
#endif /* HAVE_ROUND */

#ifndef HAVE_ROUNDF
av_always_inline float roundf(float x)
{
    return (x > 0) ? floor(x + 0.5) : ceil(x - 0.5);
}
#endif /* HAVE_ROUNDF */
