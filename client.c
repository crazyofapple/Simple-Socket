#include <sys/types.h>         
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

/* 涉及的系统调用:
 * socket
 * connect
 * send/recv
 */


#define SERVER_PORT 8188

int main(int argc, char **argv)
{
	int iSocketClient;
	struct sockaddr_in tSocketServerAddr;    /* 服务端的地址信息 */
	struct sockaddr_in tSocketClientAddr;    /* 客户端的地址信息 */      
	
	int iRet;
	unsigned char ucSendBuf[1000];
	int iSendLen;
	
	if (argc != 2)
	{
		printf("Usage:\n");
		printf("%s <server_ip>\n", argv[0]);
		return -1;
	}
	
	
	iSocketClient = socket(AF_INET, SOCK_STREAM, 0);

	tSocketServerAddr.sin_family = AF_INET;
	/* (htons: host to net, short): 把主机字节序转化为网络字节序 */
	tSocketServerAddr.sin_port   = htons(SERVER_PORT);    /* 服务器要监视的端口 */
	//tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;  
	/* inet_aton : 把字符串转化为网络IP */
	iRet = inet_aton(argv[1], &tSocketServerAddr.sin_addr);
	if (iRet == 0)
	{
		printf("invalid server_ip\n");
		return -1;
	}
	memset(tSocketServerAddr.sin_zero, 0, 8);

	/* 连接到服务器 */
	iRet = connect(iSocketClient, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
	if (iRet == -1)
	{
		printf("connect error!\n");
		return -1;
	}
	printf("ok\n");
	while (1)
	{
		if (fgets(ucSendBuf, 999, stdin))
		{
			iSendLen = send(iSocketClient, ucSendBuf, strlen(ucSendBuf), 0);
			if (iSendLen <= 0)
			{
				printf("send error!\n");
				close(iSocketClient);
				return -1;
			}
		}
	}

	
	return 0;
}
