#include <stdio.h>
#include <string.h>
typedef struct
{
    /**//* byte 0 */
    unsigned char u4CSrcLen:4;		/**//* expect 0 */
    unsigned char u1Externsion:1;	/**//* expect 1, see RTP_OP below */
    unsigned char u1Padding:1;		/**//* expect 0 */
    unsigned char u2Version:2;		/**//* expect 2 */
    /**//* byte 1 */
    unsigned char u7Payload:7;		/**//* RTP_PAYLOAD_RTSP */
    unsigned char u1Marker:1;		/**//* expect 1 */
    /**//* bytes 2, 3 */
    unsigned short u16SeqNum;
    /**//* bytes 4-7 */
    unsigned long long u32TimeStamp;
    /**//* bytes 8-11 */
    unsigned long u32SSrc;			/**//* stream number is used here. */
}StRtpFixedHdr;

int main(void)
{
	StRtpFixedHdr rtp1;
	memset(&rtp1,'\0',sizeof(rtp1));
	rtp1.u4CSrcLen=8;
	rtp1.u1Externsion=1;
	rtp1.u1Padding=0;
	rtp1.u2Version=1;

	rtp1.u7Payload=2;
	rtp1.u1Marker=1;
	unsigned char a=((unsigned char*)(&rtp1))[0];
	unsigned char b=((unsigned char*)(&rtp1))[1];
	unsigned short c=((unsigned short*)(&rtp1))[0];
	printf("a=%x\n",a);
	printf("b=%x\n",b);
	printf("c=%x\n",c);
	printf("u4CSrcLen=%x\n",rtp1.u4CSrcLen);
	printf("u1Externsion=%x\n",rtp1.u1Externsion);
	printf("u1Padding=%x\n",(rtp1.u1Padding));
	printf("u2Version=%x\n",(rtp1.u2Version));
	return 0;
}
