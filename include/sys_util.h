
#ifndef _SYS_UTIL_H_
#define _SYS_UTIL_H_ 

#ifdef __cplusplus
extern "C"
{
#endif

void IpToString(int intIP, char* pchIP);
int StringToIp(char *pchIP);
int GetSubnet(char* pIp, char* pMask);

#ifdef __cplusplus
}
#endif

#endif