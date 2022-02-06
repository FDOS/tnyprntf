
#ifdef NOPRNTF
/* Caller will need to include stdio.h */
#define PRINTF printf
#define SPRINTF sprintf
#else
int Tprintf(const char * fmt, ...);
int Tsprintf(char *, const char * fmt, ...);
#define PRINTF Tprintf
#define SPRINTF Tsprintf
#endif
