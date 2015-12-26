#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <string>

using namespace std;

void main()
{
	int r;
	WSAData wsaData;
	WORD DLLVSERION;
	DLLVSERION = MAKEWORD(2, 1);//Winsocket-DLL ����

								//�� WSAStartup �}�l Winsocket-DLL
	r = WSAStartup(DLLVSERION, &wsaData);

	//�ŧi socket ��}��T(���P���q�T,�����P����}��T,�ҥH�|�����P����Ƶ��c�s��o�Ǧ�}��T)
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);

	//�إ� socket
	SOCKET sListen; //listening for an incoming connection
	SOCKET sConnect; //operating if a connection was found

					 //AF_INET�G���ܫإߪ� socket �ݩ� internet family
					 //SOCK_STREAM�G���ܫإߪ� socket �O connection-oriented socket 
	sConnect = socket(AF_INET, SOCK_STREAM, NULL);

	//�]�w��}��T�����
	addr.sin_addr.s_addr = inet_addr("192.168.100.161");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(25565);

	//�]�w Listen
	sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);//SOMAXCONN: listening without any limit

							   //���ݳs�u
	SOCKADDR_IN clinetAddr;
	while (true)
	{
		cout << "Waiting for Connection..." << endl;

		if (sConnect = accept(sListen, (SOCKADDR*)&clinetAddr, &addrlen))
		{
			cout << "A Connection was Found" << endl;
			cout << "Server: Got Connection From: " << inet_ntoa(addr.sin_addr) << endl;
			//printf("server: got connection from %s\n", inet_ntoa(addr.sin_addr));

			//�ǰe�T���� client ��
			while (1) {
				string Msg;
				cin >> Msg;
				//string *sendbuf = Msg ;
				send(sConnect, Msg.c_str(), (int)strlen(Msg.c_str()), 0);
			}


		}
	}

	//getchar();
}