#if !defined(_log_h__)
#define _log_h__

#if DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

/* #define DEBUG_PRINT(x) printf x */

#endif

