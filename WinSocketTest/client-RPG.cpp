#include"cstdio"
#include"cstdlib"
#include"conio.h" 				//kbhit()
#include"time.h"
#include <windows.h>  			//gotoyx	setcolor()
#include <cmath>
// Winsock for our network communication
#include<WinSock2.h>
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

using namespace std;

char fireloli[200];
void gotoyx(int ypos, int xpos);
void SetColor(int f = 7, int b = 0);
void choose_color_print(char input);
void print(struct maze map);
struct maze insert_map(struct maze map);
struct maze monster_pop(struct character *monster, struct maze map);
struct maze player_move(struct character *player, struct maze map);
struct maze monster_move(struct character player, struct character *monster, struct maze map, int mode);
void blood(struct character *one, char operation, int row);
int meet(struct character player, struct character monster);
void end(int n);
struct character shoot(struct character bullet, struct character player);
struct character bullet_move(struct character bullet, struct maze *map, struct character *monster_1, struct character *monster_2, struct character *monster_3);
int monster_status(struct character monster, int row);

struct maze {
	int row, col;
	int start_row, start_col;
	char map[100][100];
};

struct character {
	int row, col;
	int blood = 20;
	int sperate = 1;
	char presign;
	char move;
	int exist = 1;
};

int main() {
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
	srand(time(NULL));
	struct maze map;
	struct maze *map_pointer = &map;
	struct character player_1, monster_1, monster_2, monster_3, bullet[1000];
	struct character *player_1_pointer = &player_1, *monster_1_pointer = &monster_1, *monster_2_pointer = &monster_2, *monster_3_pointer = &monster_3;
	char kbtmp;
	//map
	/*printf("please insert the row and col(row / col):");
	scanf("%d / %d",&map.row,&map.col);
	printf("please insert the map:\n");
	map=insert_map(map);
	printf("where to start?(row / col)");
	scanf("%d / %d",&map.start_row,&map.start_col);
	system("cls");*/
	map = insert_map(map);
	//character_player_1 origin
	player_1.row = map.start_row;
	player_1.col = map.start_col;
	player_1.presign = map.map[player_1.row][player_1.col];
	map.map[player_1.row][player_1.col] = '$';
	//character_monster_1 origin
	map = monster_pop(monster_1_pointer, map);
	//character_monster_2 origin
	map = monster_pop(monster_2_pointer, map);
	//character_monster_3 origin
	map = monster_pop(monster_3_pointer, map);
	printf("Welcome!! here are some instructions :\n");
	printf("1.please use english mode\n");
	printf("2.please use the max size of the application\n");
	printf("3.w/a/s/d stands for up/left/down/right\n");
	printf("4.you can shoot monster with direction key(w/a/s/d)+j\n");
	printf("\n");
	printf("Please click \"s\" to start game.");
	while (getch() != 's');
	system("cls");
	print(map);
	blood(player_1_pointer, '=', map.row + 2);
	while (1) {
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

		/*if (GetAsyncKeyState(83)) {
			char input[200];
			ZeroMemory(input, 200);
			HANDLE writeHandle;
			writeHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)writeProc, &pollingSet.fd_array[0], 0, NULL);
			send(unsigned int(pollingSet.fd_array[0]), input, (int)strlen(input), 0);
			WaitForSingleObject(writeHandle, INFINITE);
			*
		}*/
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
		//print(map);
		//whether player_1 move
		for (unsigned int i = 0; i < pollingSet.fd_count; i++)
		{
			// We can access the socket list directly using the fd_array member of the FD_SET
			unsigned int clientSocket = pollingSet.fd_array[i];
			int nNetTimeout = 1;//1秒，

								//設置接收超時
			setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
			setsockopt(pollingSet.fd_array[i], SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
			HANDLE readHandle;
			readHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)readProc, &clientSocket, 0, NULL);
			WaitForSingleObject(readHandle, 100);
			if (kbhit()) {
				kbtmp = fireloli[0];
			}
			else {
				_sleep(0.2 * 1000);
			}
			//player_1 shoot or move
			if (kbtmp == fireloli[0]) {
				//shoot
				int a = 0;
				while (bullet[a].exist != 1)a++;
				bullet[a] = shoot(bullet[a], player_1);
			}
			else {
				//move
				player_1.move = kbtmp;
				map = player_move(player_1_pointer, map);
			}
			kbtmp = NULL;
			//bullet move
			int a;
			for (a = 0; a < 1000; a++) {
				bullet[a] = bullet_move(bullet[a], map_pointer, monster_1_pointer, monster_2_pointer, monster_3_pointer);
			}
			//monster_1 move
			if (monster_1.exist == 1)map = monster_move(player_1, monster_1_pointer, map, 1);
			//monster_2 move
			if (monster_2.exist == 1)map = monster_move(player_1, monster_2_pointer, map, 2);
			//monster_3 move
			if (monster_3.exist == 1)map = monster_move(player_1, monster_3_pointer, map, 2);
			//blood change monster_1
			if (monster_1.exist == 1) {
				if (meet(player_1, monster_1) == 1) {
					if (monster_1.sperate == 1) {
						blood(player_1_pointer, '-', map.row + 2);
						monster_1.sperate = 0;
					}
				}
				else monster_1.sperate = 1;
			}
			//blood change monster_2
			if (monster_2.exist == 1) {
				if (meet(player_1, monster_2) == 1) {
					if (monster_2.sperate == 1) {
						blood(player_1_pointer, '-', map.row + 2);
						monster_2.sperate = 0;
					}
				}
				else monster_2.sperate = 1;
			}
			//blood change monster_3
			if (monster_3.exist == 1) {
				if (meet(player_1, monster_3) == 1) {
					if (monster_3.sperate == 1) {
						blood(player_1_pointer, '-', map.row + 2);
						monster_3.sperate = 0;
					}
				}
				else monster_3.sperate = 1;
			}
			//monster blood
			if (monster_status(monster_1, map.row + 4) + monster_status(monster_2, map.row + 6) + monster_status(monster_3, map.row + 8) == 0) {
				_sleep(1 * 1000);
				end(1);
			}
			//system("cls");
		}
		return 0;
	}
}

void print(struct maze map) {
	int a, b;
	for (a = 0; a<map.row; a++) {
		for (b = 0; b<map.col; b++) {
			choose_color_print(map.map[a][b]);
			//printf("%c",map.map[a][b]);
		}
		printf("\n");
	}
	/*int a;
	for(a=0;a<map.row;a++){
	map.map[a][map.col]='\0';
	printf("%s\n",map.map[a]);
	}*/
}

struct maze insert_map(struct maze map) {
	FILE *map_in;
	int map_num = rand() % 3 + 1;
	switch (map_num) {
	case 1:
		map_in = fopen(".\\map\\map-demo.txt", "r");
		break;
	case 2:
		map_in = fopen(".\\map\\map-demo2.txt", "r");
		break;
	case 3:
		map_in = fopen(".\\map\\map-demo3.txt", "r");
		break;
	}
	//map_in=fopen("map-demo.txt","r");
	fscanf(map_in, "%d", &map.row);
	fscanf(map_in, "%d", &map.col);
	fscanf(map_in, "%d", &map.start_row);
	fscanf(map_in, "%d", &map.start_col);
	int a, b;
	for (a = 0; a<map.row; a++) {
		for (b = 0; b<map.col; b++) {
			fscanf(map_in, "%c", &map.map[a][b]);
			// /./0/@/*/)
			//if(map.map[a][b]=='\n')b=b-1;
			if (map.map[a][b] != '#'&&map.map[a][b] != ' '&&map.map[a][b] != '.'&&map.map[a][b] != '0'&&
				map.map[a][b] != '@'&&map.map[a][b] != '*'&&map.map[a][b] != ')')b -= 1;
			//printf("%d %d %c\n",a,b,map.map[a][b]);
		}
	}
	fclose(map_in);
	return map;
}

struct maze monster_pop(struct character *monster, struct maze map) {
	while (1) {
		monster->row = rand() % map.row;
		monster->col = rand() % map.col;
		if (map.map[monster->row][monster->col] != ' '&&map.map[monster->row][monster->col] != '*')continue;
		else break;
	}
	monster->presign = map.map[monster->row][monster->col];
	if (map.map[monster->row][monster->col] == '*')map.map[monster->row][monster->col] = '&';
	else map.map[monster->row][monster->col] = '%';
	return map;
}

struct maze player_move(struct character *player, struct maze map) {
	if (player->presign == '*')map.map[player->row][player->col] = '*';
	else map.map[player->row][player->col] = ' ';
	gotoyx(player->row, player->col);
	//printf("%c",map.map[player->row][player->col]);
	choose_color_print(map.map[player->row][player->col]);
	switch (player->move) {
	case 'w':
		if (map.map[player->row - 1][player->col] == ' ' || map.map[player->row - 1][player->col] == '*')player->row -= 1;
		break;
	case 's':
		if (map.map[player->row + 1][player->col] == ' ' || map.map[player->row + 1][player->col] == '*')player->row += 1;
		break;
	case 'a':
		if (map.map[player->row][player->col - 1] == ' ' || map.map[player->row][player->col - 1] == '*')player->col -= 1;
		break;
	case 'd':
		if (map.map[player->row][player->col + 1] == ' ' || map.map[player->row][player->col + 1] == '*')player->col += 1;
		break;
	}
	player->presign = map.map[player->row][player->col];
	if (player->presign == '*')map.map[player->row][player->col] = '&';
	else map.map[player->row][player->col] = '$';
	gotoyx(player->row, player->col);
	//printf("%c",map.map[player->row][player->col]);
	choose_color_print(map.map[player->row][player->col]);
	return map;
}

struct maze monster_move(struct character player, struct character *monster, struct maze map, int mode) {
	if (monster->presign == '*')map.map[monster->row][monster->col] = '*';
	else map.map[monster->row][monster->col] = ' ';
	gotoyx(monster->row, monster->col);
	//printf("%c",map.map[monster->row][monster->col]);
	choose_color_print(map.map[monster->row][monster->col]);
	if (mode == 1) {
		monster->move = rand() % 4;
	}
	else if (mode == 2) {
		monster->move = rand() % 8;
		if (player.col <= monster->col) {
			if (player.row <= monster->row) {
				if (monster->move == 4 || monster->move == 5)monster->move = 0;
				if (monster->move == 6 || monster->move == 7)monster->move = 2;
			}
			else {
				if (monster->move == 4 || monster->move == 5)monster->move = 1;
				if (monster->move == 6 || monster->move == 7)monster->move = 2;
			}
		}
		else {
			if (player.row <= monster->row) {
				if (monster->move == 4 || monster->move == 5)monster->move = 0;
				if (monster->move == 6 || monster->move == 7)monster->move = 3;
			}
			else {
				if (monster->move == 4 || monster->move == 5)monster->move = 1;
				if (monster->move == 6 || monster->move == 7)monster->move = 3;
			}
		}
	}
	switch (monster->move) {
	case 0:
		if (map.map[monster->row - 1][monster->col] == ' ' || map.map[monster->row - 1][monster->col] == '*')monster->row -= 1;
		break;
	case 1:
		if (map.map[monster->row + 1][monster->col] == ' ' || map.map[monster->row + 1][monster->col] == '*')monster->row += 1;
		break;
	case 2:
		if (map.map[monster->row][monster->col - 1] == ' ' || map.map[monster->row][monster->col - 1] == '*')monster->col -= 1;
		break;
	case 3:
		if (map.map[monster->row][monster->col + 1] == ' ' || map.map[monster->row][monster->col + 1] == '*')monster->col += 1;
		break;
	}
	monster->presign = map.map[monster->row][monster->col];
	if (monster->presign == '*')map.map[monster->row][monster->col] = '&';
	else map.map[monster->row][monster->col] = '%';
	gotoyx(monster->row, monster->col);
	//printf("%c",map.map[monster->row][monster->col]);
	choose_color_print(map.map[monster->row][monster->col]);
	return map;
}

void gotoyx(int ypos, int xpos) {
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos; scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput, scrn);
}

void SetColor(int f, int b) {
	unsigned short ForeColor = f + 16 * b;
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, ForeColor);
}

void choose_color_print(char input) {
	int special_print = 0;
	switch (input) {
	case'#':
		SetColor(14, 0);
		break;
	case'*':
		SetColor(3, 0);
		break;
	case'0':
		SetColor(15, 0);
		break;
	case'@':
		SetColor(10, 0);
		break;
	case')':
		SetColor(9, 0);
		break;
	case'$':
		SetColor(13, 0);
		break;
	case'%':
		SetColor(12, 0);
		break;
	case'&':
		SetColor(11, 0);
		printf("*");
		special_print = 1;
		break;
	case'b':
		SetColor(0, 12);
		printf("  ");
		SetColor();
		printf(" ");
		special_print = 1;
		break;
	case'd':
		SetColor(0, 7);
		printf("  ");
		SetColor();
		printf(" ");
		special_print = 1;
		break;
	}
	if (special_print == 0)printf("%c", input);
	SetColor();
}

void blood(struct character *one, char operation, int row) {
	switch (operation) {
	case'=':
		break;
	case'+':
		if (one->blood<10)one->blood += 1;
		break;
	case'-':
		one->blood -= 1;
		break;
	}
	//print blood
	int a;
	gotoyx(row, 0);
	printf("%d\%\t", one->blood * 5);
	for (a = 0; a<one->blood; a++)choose_color_print('b');
	for (; a<20; a++)choose_color_print('d');
	if (one->blood == 0) {
		_sleep(1 * 1000);
		end(0);
	}
}

int meet(struct character player, struct character monster) {
	if (abs(player.row - monster.row) == 1 && abs(player.col - monster.col) == 0)return 1;
	else if (abs(player.row - monster.row) == 0 && abs(player.col - monster.col) == 1)return 1;
	else return 0;
}

void end(int n) {
	system("cls");
	switch (n) {
	case 1:
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\t\t#  #  # ##### ##   #\n");
		printf("\t\t#  #  #   #   # #  #\n");
		printf("\t\t#  #  #   #   #  # #\n");
		printf("\t\t ## ##  ##### #   ##\n");
		break;
	case 0:
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\t\t#     ###   ####  #####\n");
		printf("\t\t#    #   # #      #    \n");
		printf("\t\t#    #   #  ####  #####\n");
		printf("\t\t#    #   #      # #    \n");
		printf("\t\t####  ###   ####  #####\n");
		break;
	}
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	system("pause");
	exit(0);
}

struct character shoot(struct character bullet, struct character player) {
	bullet.row = player.row;
	bullet.col = player.col;
	bullet.move = player.move;
	bullet.exist = 2;
	return bullet;
}

struct character bullet_move(struct character bullet, struct maze *map, struct character *monster_1, struct character *monster_2, struct character *monster_3) {
	if (bullet.exist == 2) {
		for (int a = 0; a<2; a++) {
			map->map[bullet.row][bullet.col] = ' ';
			gotoyx(bullet.row, bullet.col);
			printf("%c", map->map[bullet.row][bullet.col]);
			switch (bullet.move) {
			case'w':
				bullet.row -= 1;
				break;
			case's':
				bullet.row += 1;
				break;
			case'a':
				bullet.col -= 1;
				break;
			case'd':
				bullet.col += 1;
				break;
			default:
				bullet.exist = 0;
				break;
			}
			if (map->map[bullet.row][bullet.col] != ' '&&map->map[bullet.row][bullet.col] != '*') {
				if (map->map[bullet.row][bullet.col] == '%') {
					if (bullet.row == monster_1->row&&bullet.col == monster_1->col)monster_1->exist = 0;
					if (bullet.row == monster_2->row&&bullet.col == monster_2->col)monster_2->exist = 0;
					if (bullet.row == monster_3->row&&bullet.col == monster_3->col)monster_3->exist = 0;
					map->map[bullet.row][bullet.col] = ' ';
					gotoyx(bullet.row, bullet.col);
					printf("%c", map->map[bullet.row][bullet.col]);
				}
				bullet.exist = 0;
				break;
			}
			else {
				map->map[bullet.row][bullet.col] = '.';
				gotoyx(bullet.row, bullet.col);
				printf("%c", map->map[bullet.row][bullet.col]);
			}
		}
	}
	return bullet;
}

int monster_status(struct character monster, int row) {
	gotoyx(row, 0);
	printf("monster\t");
	if (monster.exist == 1)choose_color_print('b');
	else choose_color_print('d');
	return monster.exist;
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

	//cout << message << endl;
	strcpy(fireloli, message);
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