
#ifndef _SYS_PORT_H_
#define _SYS_PORT_H_

/* Include necessary headers for osip */

#include <stdio.h>

#if defined(__PALMOS__) && (__PALMOS__ >= 0x06000000)
#	define STDC_HEADERS 1
#	define HAVE_CTYPE_H 1
#	define HAVE_STRING_H 1
#	define HAVE_SYS_TYPES_H 1
#	define HAVE_TIME_H 1
#	define HAVE_STDARG_H 1

#elif defined(__LINUX__)

#	define HAVE_PTH_PTHREAD_H 1
#	define HAVE_SEMAPHORE_H 1
#	define HAVE_CONFIG_H 1
#	define HAVE_FCNTL_H 1
#	define HAVE_SYS_TIME_H 1
#	define SYS_MT 1
#	define ENABLE_TRACE 1

#elif defined(WIN32)

#	define STDC_HEADERS 1
#	define HAVE_CTYPE_H 1
#	define HAVE_STRING_H 1
#	define HAVE_SYS_TYPES_H 1
#	define HAVE_TIME_H 1
#	define HAVE_STDARG_H 1
#	define SYS_MT 1
#	define ENABLE_TRACE 1

/* use win32 crypto routines for random number generation */
/* only use for vs .net (compiler v. 1300) or greater */
#if _MSC_VER >= 1300
#define WIN32_USE_CRYPTO 1
#endif

#elif defined _WIN32_WCE

#define STDC_HEADERS 1
#define HAVE_CTYPE_H 1
#define HAVE_STRING_H 1
#define HAVE_TIME_H 1
#define HAVE_STDARG_H 1

#define strnicmp	_strnicmp
#define stricmp		_stricmp
#define EBUSY           16

#endif

#ifdef __VXWORKS_OS__
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdlib.h>
#define VA_START(a, f)  va_start(a, f)

/* VxWorks lacks support for snprintf */
int sys_vsnprintf( char* buf, int max, const char *fmt, va_list ap);
int sys_snprintf(  char *buf, int max, const char *fmt, ...);

#define snprintf  sys_snprintf
#define vsnprintf sys_vsnprintf


#else /* end of __VXWORKS_OS__ */

#if defined (HAVE_CONFIG_H)
#include <config.h>
#  if defined (HAVE_STRING_H)
#    include <string.h>
#  else
#    include <strings.h>
#  endif /* HAVE_STRING_H */
#else
#  include <string.h>
#endif /* !HAVE_CONFIG_H */

#if defined (HAVE_SYS_TYPES_H)
#  include <sys/types.h>
#endif

#if STDC_HEADERS
#    include <stdlib.h>
#endif /* !STDC_HEADERS */

#if defined(HAVE_STDARG_H) || defined(WIN32)
#  include <stdarg.h>
#  define VA_START(a, f)  va_start(a, f)
#else
#  if defined(HAVE_VARARGS_H)
#    include <varargs.h>
#    define VA_START(a, f) va_start(a)
#  else
#    include <stdarg.h>
#    define VA_START(a, f)  va_start(a, f)
#  endif
#endif

#ifdef HAVE_TIME_H
#  include <time.h>
#endif

#if defined (HAVE_SYS_TIME_H)
#  include <sys/time.h>
#endif

#endif /* end of !__VXWORKS_OS__ */

#ifdef _WIN32_WCE
#define VA_START(a, f)  va_start(a, f)
#endif

#ifdef WIN32
#define VA_START(a, f)  va_start(a, f)
#endif

#ifdef __PSOS__
#define VA_START(a, f)  va_start(a, f)
#endif

#if __STDC__
#  ifndef NOPROTOS
#    define PARAMS(args)   args
#  endif
#endif

#ifndef PARAMS
#  define PARAMS(args)     ()
#endif

#define SIP_SYNTAX_ERROR    (-1)
#define SIP_NETWORK_ERROR   (-2)
#define SIP_ECONNREFUSED    (-3)
#define SIP_RESSOURCE_ERROR (-4)
#define SIP_GLOBAL_ERROR    (-5)

#ifdef __cplusplus
extern "C"
{
#endif

/**************************/
/* MALLOC redirections    */
/**************************/
#ifndef WIN32
typedef void *sys_malloc_func_t(size_t size);
typedef void sys_free_func_t(void *ptr);
typedef void *sys_realloc_func_t(void *ptr, size_t size);

extern sys_malloc_func_t  *sys_malloc_func;
extern sys_realloc_func_t *sys_realloc_func;
extern sys_free_func_t    *sys_free_func;

void sys_set_allocators(sys_malloc_func_t  *malloc_func, 
                         sys_realloc_func_t *realloc_func, 
                         sys_free_func_t    *free_func);

#ifndef sys_malloc
#define sys_malloc(S) (sys_malloc_func?sys_malloc_func(S):malloc(S))
#endif
#ifndef sys_realloc
#define sys_realloc(P,S) (sys_realloc_func?sys_realloc_func(P,S):realloc(P,S))
#endif
#ifndef sys_free
#define sys_free(P) { if (P!=NULL) { if (sys_free_func) sys_free_func(P); else free(P);} }
#endif

# include "memwatch.h"

#else
void *sys_malloc(size_t size);
void *sys_realloc(void *, size_t size);
void sys_free(void *);
#endif

#ifdef WIN32
#define vsnprintf	_vsnprintf
#define snprintf   _snprintf
#define alloca _alloca
#endif

/**************************/
/* RANDOM number support  */
/**************************/

  unsigned int sys_build_random_number (void);

/**************************/
/* TIMER support          */
/**************************/

#define SP   " \0"

  void sys_usleep (int useconds);

/**************************/
/* STRING support         */
/**************************/

  int sys_atoi (const char *number);
  char *sys_strncpy (char *dest, const char *src, size_t length);
  char *sys_strdup (const char *ch);
  char *sys_strdup_without_quote (const char *ch);
  int sys_tolower (char *word);
  int sys_clrspace (char *word);
  char *__sys_sdp_append_string (char *string, size_t size,
				  char *cur, char *string_sys_to_append);
  int __sys_set_next_token (char **dest, char *buf, int end_separator,
			     char **next);
  /* find the next unescaped quote and return its index. */
  char *__sys_quote_find (const char *qstring);
  char *sys_enquote (const char *s);
  void sys_dequote (char *s);

  int sys_strcasecmp (const char *s1, const char *s2);
  int sys_strncasecmp (const char *s1, const char *s2, size_t len);

/**************************/
/* LOG&DEBUG support      */
/**************************/

#define LOG_TRUE  1
#define LOG_FALSE 0
/* levels */
  typedef enum _trace_level
  {
    SYS_FATAL = 0,
    SYS_BUG = 1,
    SYS_ERROR = 2,
    SYS_WARNING = 3,
    SYS_INFO1 = 4,
    SYS_INFO2 = 5,
    SYS_INFO3 = 6,
    SYS_INFO4 = 7,
    SYS_INFO5 = 8,
    SYS_INFO6 = 9,
    SYS_INFO7 = 10,
    SYS_INFO8 = 11,
    END_TRACE_LEVEL = 12
  }
  sys_trace_level_t;


typedef void sys_trace_func_t(char *fi, int li, sys_trace_level_t level, char *chfr, va_list ap);

/* these are defined in all cases, but are empty when oSIP is compiled
   without trace */
  void sys_trace_initialize_func (sys_trace_level_t level, sys_trace_func_t *func);
  void sys_trace_initialize_syslog (sys_trace_level_t level, char *ident);
  void sys_trace_initialize (sys_trace_level_t level, FILE * file);
  void sys_trace_enable_until_level (sys_trace_level_t level);
  void sys_trace_enable_level (sys_trace_level_t level);
  void sys_trace_disable_level (sys_trace_level_t level);
  int sys_is_trace_level_activate (sys_trace_level_t level);
  
  void sys_trace_initialize_ext(sys_trace_level_t level, char  *FileNamePrefix);

#define __OFF___ -1

/* log facility. */
/* INPUT: level | level of the trace               */
/* INPUT: chfr | format string for next args       */
  int sys_trace (char *fi, int li, sys_trace_level_t level, char *chfr, ...);

#ifdef ENABLE_TRACE
#define _SYS_TRACE(MSG) do {} while (0)
#else
#define _SYS_TRACE(MSG) do {} while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif				/* _PORT_H_ */
