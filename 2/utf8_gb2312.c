#include "utf8_gb2312.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iconv.h>
#include <string.h>

int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
 iconv_t cd;
 char **pin = &inbuf;
 char **pout = &outbuf;
 char *old;
 old = outbuf;
 cd = iconv_open(to_charset,from_charset);
 //printf("cd:%d\n",cd);
 if (cd==0) return -1;
 memset(outbuf,0,outlen);
 if (iconv(cd,pin,&inlen,pout,&outlen)==-1)
 {
  return -1;
 }
 iconv_close(cd);
 return outbuf - old;
}


int  gb2312_utf8(char *inbuf,int inlen,char *outbuf,int outlen)
{
 return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}


int  utf8_gb2312(char *inbuf,int inlen,char *outbuf,int outlen)
{
 return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}


int  gb2312_ucs2(char *inbuf,int inlen,char *outbuf,int outlen)
{
 return code_convert("gb2312","ucs-2",inbuf,inlen,outbuf,outlen);
}


int  ucs2_gb2312(char *inbuf,int inlen,char *outbuf,int outlen)
{
 return code_convert("ucs-2","gb2312",inbuf,inlen,outbuf,outlen);
}
