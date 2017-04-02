#include <sys/types.h>         
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

/* 涉及的系统调用:
 * socket
 * bind
 * listen
 * accept
 * send/recv
 */

#define SERVER_PORT 8188
#define BACKLOG     10       /* 最多可以监听多少路连接 */

int main(int argc, char **argv)
{
	int iSocketServer;      /* 服务端描述符 */
	int iSocketClient;      /* 客户端描述符 */
	struct sockaddr_in tSocketServerAddr;    /* 服务端的地址信息 */
	struct sockaddr_in tSocketClientAddr;    /* 客户端的地址信息 */
	int iRet;
	int iAddrLen;

	int iRecvLen;
	unsigned char ucRecvBuf[1000];     /* 缓冲区 */

	int iClientNum = -1;                /* 表示是哪个客户端 */

	signal(SIGCHLD, SIG_IGN);
	
	iSocketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	tSocketServerAddr.sin_family = AF_INET;
	
	/* (htons: host to net, short): 把主机字节序转化为网络字节序 */
	tSocketServerAddr.sin_port   = htons(SERVER_PORT);    /* 服务器要监视的端口 */

	tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;       /* 本机上所有的 IP */

	memset(tSocketServerAddr.sin_zero, 0, 8);
	
	iRet = bind(iSocketServer, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	iRet = listen(iSocketServer, BACKLOG);
	if (-1 == iRet)
	{
		printf("listen error!\n");
		return -1;
	}
	
	while (1)
	{
		iAddrLen = sizeof(struct sockaddr);
		iSocketClient = accept(iSocketServer, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
		if (iSocketClient != -1)
		{
			iClientNum++;
			
			/* inet_ntoa: 将一个IP转换成一个互联网标准点分格式的字符串 */
			printf("Get connect from client %d : %s\n", iClientNum, inet_ntoa(tSocketClientAddr.sin_addr));

			/* 如果有多个客户端，则每个客户端都创建一个子进程 */
			if (!fork())    /* 创建子进程 */
			{
				/* 子进程的源码 */
				while (1)
				{
					/* 接收客户端发来的数据并显示出来 */
					iRecvLen = recv(iSocketClient, ucRecvBuf, 999, 0);
					if (iRecvLen <= 0)
					{
						close(iSocketClient);
						return -1;
					}
					else
					{
						ucRecvBuf[iRecvLen] = '\0';
						printf("Get Msg From Client %d : %s\n", iClientNum, ucRecvBuf);
					}
				}
			}
		}
	}

	close(iSocketServer);
	return 0;
}
