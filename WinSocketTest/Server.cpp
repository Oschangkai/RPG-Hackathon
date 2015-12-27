// Winsock for our network communication
#include <winsock2.h>
// Stdio for I/O
#include <stdio.h>
#include<iostream>
using namespace std;
void readProc(int *sConnect);
void writeProc(int *sConnect);

// -----------------------------------------------------------------------------------
// startupServerForListening() - will return us a socket that is bound to the
// port we specify, and is listening for connections if all operations succeeded,
// and a -1 if startup failed.
int startupServerForListening(unsigned short port);

// -----------------------------------------------------------------------------------
// shutdownServer() - will take the socket we specify and shutdown the 
// network utilities we started with startupServerForListening()
// Note: In order to function properly, the socket passed in MUST be the
// socket created by the startupServerForListening() function.
void shutdownServer(int socket);

// For threading, we will need a thread handle.  We will receive this handle when we create the thread
// and can use this handle any time we need to reference our thread.
HANDLE threadHandle;

// A mutex for my shared data
HANDLE mutexHandle;

// My master socket set that will be protected by my mutex
FD_SET masterSet;

// A quit flag I will use to exit my program with if something goes wrong
bool gQuitFlag = false;
int r;
void acceptingThreadProcedure(int* serverSocket)
{
	// Copy my socket over to a local variable
	int mySocket = *serverSocket;

	// Run forever
	for (;;)
	{
		// Accept a client like normal
		unsigned int clientSocket = accept(mySocket, 0, 0);
		int nNetTimeout = 1;//1秒，

							//設置接收超時
		setsockopt(mySocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
		setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
		// Make sure things are ok
			// Lock the mutex
			WaitForSingleObject(mutexHandle, INFINITE);

			// Add this socket to the master set using our FD_SET() macro
			FD_SET(clientSocket, &masterSet);

			// Unlock the mutex
			ReleaseMutex(mutexHandle);

			// A quick message
			::printf("client on %d connected\n", clientSocket);
		}
	}


int main()
{
	::printf("Welcome to redKlyde's Networking Tutorials!\n");
	::printf("Tutorial # 5 : Multiple Hosts - ServerSide\n\n");

	// Startup our network as usual.

	// The socket my server will use for listening
	int serverSocket;

	// Startup my network utilities with my handy functions
	serverSocket = startupServerForListening(25565);
	int nNetTimeout = 1;//1秒，

						//設置接收超時
	setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));

	// Check for errors
	if (serverSocket == -1)
	{
		::printf("Network Startup Failed!\nProgram Terminating\n");
		return 1;
	}

	// Create the mutex
	mutexHandle = CreateMutex(NULL, false, NULL);

	if (mutexHandle == NULL)
	{
		::printf("Error creating mutex\n");
		shutdownServer(serverSocket);
		return 1;
	}

	// Create the thread
	int threadId;
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)acceptingThreadProcedure, &serverSocket, 0, (LPDWORD)&threadId);
	setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));

	if (threadHandle == NULL)
	{
		::printf("Could not start acceptance thread\n");
		shutdownServer(serverSocket);
		return 1;
	}

	// Sleep the main() so the acceptance thread has time to start ... this is cheesy
	Sleep(100);

	// Now that that is all over with, it's down to business.

	// It's always a good idea to initialize our structures before we access them, so let's zero our masterSet.
	FD_ZERO(&masterSet);

	/*int nNetTimeout = 2;//1秒，

						//設置接收超時
	setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));*/

	// The main loop ... run forever
	cout << "Before for";
	for (;;)
	{
		if (gQuitFlag)
		{
			break;
		}

		// Lock the mutex
		WaitForSingleObject(mutexHandle, INFINITE);


		// Make the polling set and copy everything from masterSet
		FD_SET pollingSet = masterSet;

		// Unlock the mutex
		ReleaseMutex(mutexHandle);

		// Check if our set is empty
		if (pollingSet.fd_count == 0)
		{
			continue;
		}

		if (GetAsyncKeyState(83)) {
			char input[200];
			ZeroMemory(input, 200);
			HANDLE writeHandle;
			writeHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)writeProc, &pollingSet.fd_array[0], 0, NULL);
			send(unsigned int(pollingSet.fd_array[0]), input, (int)strlen(input), 0);
			WaitForSingleObject(writeHandle, INFINITE);

		}
		// The wait time
		timeval waitTime;
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 0;

		// Let's select from the polling set using fd_count as the number of sockets.  We do not have a
		// write set nor an errors set, so just pass NULL for them.

		int result = select(pollingSet.fd_count, &pollingSet, NULL, NULL, &waitTime);
		//cout << pollingSet.fd_count << endl;

		// Check for no sockets with data
		if (result == 0)
		{
			// No sockets have data
			continue;
		}

		// Check for errors
		if (result == SOCKET_ERROR)
		{
			::printf("Error in select()\n");
			continue;
		}

		// Now that we have the polling set that contains just the sockets that have data, let's step through
		// the list of sockets and read from them all.


		// For every socket in my polling set
		for (unsigned int i = 0; i < pollingSet.fd_count; i++)
		{
			// We can access the socket list directly using the fd_array member of the FD_SET
			unsigned int clientSocket = pollingSet.fd_array[i];
			int nNetTimeout = 1;//1秒，

								//設置接收超時
			setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
			setsockopt(pollingSet.fd_array[i], SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
			if (GetAsyncKeyState(83)) {
				HANDLE writeHandle;
				writeHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)writeProc, &clientSocket, 0, NULL);
				WaitForSingleObject(writeHandle, INFINITE);
				//writeHandle = FALSE;
				//CloseHandle(writeHandle);
				//HeapFree(GetProcessHeap(), 0, NULL);
			}
			else {
				/*int nNetTimeout = 1;//1秒，

									//設置接收超時
				setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));


				char message[200];
				ZeroMemory(message, 200);
				int nBytes = recv(clientSocket, message, 200, MSG_WAITALL);
				if (nBytes < 0)continue;

				cout << message << "\nmessage from " << clientSocket << endl;*/

				HANDLE readHandle;
				readHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)readProc, &clientSocket, 0, NULL);
				WaitForSingleObject(readHandle, 100);
				//readHandle = FALSE;
				//CloseHandle(readHandle);
				//HeapFree(GetProcessHeap(), 0, NULL);
				//cout << "ELSE\n";
			}
		}
	}

	// Cleanup
	shutdownServer(serverSocket);

	::printf("Press Enter to Exit ...\n");
	getchar();

	return 0;
}


// -----------------------------------------------------------------------------------
// startupServerForListening() - a function to startup winsock, and open a socket for listening

int startupServerForListening(unsigned short port)
{
	// An error code we will use to get more information about our errors
	int error;

	// The winsock data structure
	WSAData wsaData;

	// Startup winsock
	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) == SOCKET_ERROR)
	{
		::printf("Could Not Start Up Winsock!\n");
		return -1;
	}

	// Create my socket
	int mySocket = socket(AF_INET, SOCK_STREAM, 0);

	// Make sure nothing bad happened
	if (mySocket == SOCKET_ERROR)
	{
		::printf("Error Opening Socket!\n");
		return -1;
	}

	// The address structure
	struct sockaddr_in server;

	// Fill the address structure with appropriate data
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	// Now bind my socket
	if (bind(mySocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		::printf("Bind Failed!\n");
		closesocket(mySocket);
		return -1;
	}

	// Mark my socket for listening
	if (listen(mySocket, 5) == SOCKET_ERROR)
	{
		::printf("Listen Failed!\n");
		closesocket(mySocket);
		return -1;
	}

	::printf("Server Started\n");

	return mySocket;
}


// -----------------------------------------------------------------------------------
// shutdownServer() - a function to shutdown a socket and clean up winsock

void shutdownServer(int socket)
{

	// Kill my thread and my handle
	WaitForSingleObject(threadHandle, INFINITE);
	CloseHandle(threadHandle);
	CloseHandle(mutexHandle);

	// Close our socket
	closesocket(socket);

	// Shut down winsock
	WSACleanup();

	::printf("Server Shutdown\n");
}
void readProc(int *sConnect)
{
	int nNetTimeout = 1;//1秒，

						//設置接收超時
	setsockopt(*sConnect, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
	char message[200];
	//接收 server 端的訊息
	ZeroMemory(message, 200);
	r = recv(*sConnect, message, sizeof(message), MSG_WAITALL);
	//cout << "B";
	if (r < 0)return;

	cout << message << endl;
	//cout << "F";
	//FD_ZERO(&fdread);cout
	//cout << "listen\n";
	//ExitThread(0);
	return;
}
void writeProc(int *sConnect) {
	cout << "input\n";
	char message[200];
	cin >> message;
	//char *s_input = new char[input.size() + 1];
	//strcpy(s_input, input.c_str());
	send(*sConnect, message, (int)strlen(message), 0);
	return;

}