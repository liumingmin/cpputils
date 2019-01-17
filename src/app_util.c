#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "sys_util.h"

////////////////////////////////////////////////////////////////////////////////////////////////


void IpToString(int intIP, char* pchIP)
{
	unsigned char *pByte;

	pByte = (unsigned char *)&intIP;
	sprintf(pchIP, "%d.%d.%d.%d", (int)pByte[0], (int)pByte[1], (int)pByte[2], (int)pByte[3]);
}

int StringToIp(char *pchIP)
{
	int ip[4], i, result;
	unsigned char buf[4];

	if( sscanf( pchIP, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3] ) == 4 )
	{
		for (i=0; i<4; i++)
			buf[i] = (unsigned char)ip[i];
		memcpy(&result, buf, 4);
		return result;
    }
	else
		return -1;  
}

int GetSubnet(char* pIp, char* pMask)
{
	int intIp, intMask;

	intIp = StringToIp(pIp);
	intMask = StringToIp(pMask);

	return (intIp & intMask);
}
