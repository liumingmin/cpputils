#ifndef _SYS_TYPE_H_
#define _SYS_TYPE_H_

#include <stdio.h>
#include <string.h>
#include <stddef.h>

//#define NULL 0;

/* ASN.1 Primitive Type Definitions */	
typedef char            SYSCHAR;
typedef unsigned char   SYSOCTET;
typedef SYSOCTET       SYSBOOL;
typedef signed char     SYSINT8;
typedef unsigned char   SYSUINT8;
typedef int             SYSINT;
typedef unsigned int    SYSUINT;
typedef SYSINT         SYSENUM;
typedef double          SYSREAL;
typedef short           SYSSINT;
typedef unsigned short  SYSUSINT;
typedef SYSUINT        SYSTAG;
#define SYSTAG_LSHIFT  24
typedef SYSUSINT       SYS16BITCHAR;
typedef SYSUINT        SYS32BITCHAR;
typedef void*           SYSANY;

/* Error Code Constants */
#define SYS_OK            0      /* normal completion status             */
#define SYS_OK_FRAG       2      /* message fragment detected            */
#define SYS_E_BUFOVFLW   -1      /* encode buffer overflow               */
#define SYS_E_ENDOFBUF   -2      /* unexpected end of buffer on decode   */
#define SYS_E_IDNOTFOU   -3      /* identifer not found                  */
#define SYS_E_INVOBJID   -4      /* invalid object identifier            */
#define SYS_E_INVLEN     -5      /* invalid field length                 */
#define SYS_E_INVENUM    -6      /* enumerated value not in defined set  */
#define SYS_E_SETDUPL    -7      /* duplicate element in set             */
#define SYS_E_SETMISRQ   -8      /* missing required element in set      */
#define SYS_E_NOTINSET   -9      /* element not part of set              */
#define SYS_E_SEQOVFLW   -10     /* sequence of field overflow           */
#define SYS_E_INVOPT     -11     /* invalid option encountered in choice */
#define SYS_E_NOMEM      -12     /* no dynamic memory available          */
#define SYS_E_INVHEXS    -14     /* invalid hex string                   */
#define SYS_E_INVBINS    -15     /* invalid binary string                */
#define SYS_E_INVREAL    -16     /* invalid real value                   */
#define SYS_E_STROVFLW   -17     /* octet or bit string field overflow   */
#define SYS_E_BADVALUE   -18     /* invalid value specification          */
#define SYS_E_UNDEFVAL   -19     /* no def found for ref'd defined value */
#define SYS_E_UNDEFTYP   -20     /* no def found for ref'd defined type  */
#define SYS_E_BADTAG     -21     /* invalid tag value                    */
#define SYS_E_TOODEEP    -22     /* nesting level is too deep            */
#define SYS_E_CONSVIO    -23     /* value constraint violation           */
#define SYS_E_RANGERR    -24     /* invalid range (lower > upper)        */
#define SYS_E_ENDOFFILE  -25     /* end of file on file decode           */
#define SYS_E_INVUTF8    -26     /* invalid UTF-8 encoding               */
#define SYS_E_CONCMODF   -27     /* Concurrent list modification         */
#define SYS_E_ILLSTATE   -28     /* Illegal state error                  */
#define SYS_E_OUTOFBND   -29     /* out of bounds (of array, etc)        */
#define SYS_E_INVPARAM   -30     /* invalid parameter                    */
#define SYS_E_INVFORMAT  -31     /* invalid time string format           */
#define SYS_E_NOTINIT    -32     /* not initialized                      */
#define SYS_E_TOOBIG     -33     /* value is too big for given data type */
#define SYS_E_INVCHAR    -34     /* invalid character (not in char set)  */
#define SYS_E_XMLSTATE   -35     /* XML state error                      */
#define SYS_E_XMLPARSE   -36     /* XML parse error                      */
#define SYS_E_SEQORDER   -37     /* SEQUENCE elements not in order       */
#define SYS_E_INVINDEX   -38     /* invalid index for TC id              */
#define SYS_E_INVTCVAL   -39     /* invalid value for TC field           */
#define SYS_E_FILNOTFOU  -40     /* file not found                       */
#define SYS_E_FILEREAD   -41     /* error occurred reading file          */
#define SYS_E_FILEWRITE  -42     /* error occurred writing file          */
#define SYS_E_INVBASE64  -43     /* invalid base64 encoding              */
#define SYS_E_INVSOCKET  -44     /* invalid socket operation             */
#define SYS_E_XMLLIBNFOU -45     /* XML library is not found             */
#define SYS_E_XMLLIBINV  -46     /* XML library is invalid               */
#define SYS_E_NOTSUPP    -99     /* non-supported ASN construct          */
#define SYS_K_INDEFLEN   -9999   /* indefinite length message indicator  */

#define SYS_E_UNKNOW   -1

#ifndef SYSMAX
#define SYSMAX(a,b)        (((a)>(b))?(a):(b))
#endif

#ifndef SYSMIN
#define SYSMIN(a,b)        (((a)<(b))?(a):(b))
#endif

#ifndef FALSE
#define FALSE           0
#define TRUE            1
#endif
	
#endif /* #ifndef _SYS_TYPE_H_ */