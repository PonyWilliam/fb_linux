#ifndef UTF8_GB2312_H
#define UTF8_GB2312_H


int code_convert(char *from_charset,char *to_charset,char *inbuf,
                int inlen,char *outbuf,int outlen);

int  gb2312_utf8(char *inbuf,int inlen,char *outbuf,int outlen);

int  utf8_gb2312(char *inbuf,int inlen,char *outbuf,int outlen);

int  gb2312_ucs2(char *inbuf,int inlen,char *outbuf,int outlen);

int  ucs2_gb2312(char *inbuf,int inlen,char *outbuf,int outlen);

#endif