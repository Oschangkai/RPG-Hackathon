
#include <winsock2.h>

#include <windows.h>

#define IDC_EDIT_IN                 101

#define IDC_EDIT_OUT                102

#define IDC_MAIN_BUTTON             103

#define WM_SOCKET                   104







int nPort = 1688;



HWND hEditIn = NULL;

HWND hEditOut = NULL;

SOCKET Socket;

char szHistory[10000];

sockaddr sockAddrClient;



LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)

{

	WNDCLASSEX wClass;

	ZeroMemory(&wClass, sizeof(WNDCLASSEX));

	wClass.cbClsExtra = 0;

	wClass.cbSize = sizeof(WNDCLASSEX);

	wClass.cbWndExtra = 0;

	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	wClass.hIcon = NULL;

	wClass.hIconSm = NULL;

	wClass.hInstance = hInst;

	wClass.lpfnWndProc = (WNDPROC)WinProc;

	wClass.lpszClassName = "Window Class";

	wClass.lpszMenuName = NULL;

	wClass.style = CS_HREDRAW | CS_VREDRAW;



	if (!RegisterClassEx(&wClass))

	{

		int nResult = GetLastError();

		MessageBox(NULL,

			"Window class creation failed\r\nError code:",

			"Window Class Failed",

			MB_ICONERROR);

	}



	HWND hWnd = CreateWindowEx(0,

		"Window Class",

		"Winsock Async Server",

		WS_OVERLAPPEDWINDOW,

		200,

		200,

		640,

		480,

		NULL,

		NULL,

		hInst,

		NULL);



	if (!hWnd)

	{

		int nResult = GetLastError();



		MessageBox(NULL,

			"Window creation failed\r\nError code:",

			"Window Creation Failed",

			MB_ICONERROR);

	}



	ShowWindow(hWnd, nShowCmd);



	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));



	while (GetMessage(&msg, NULL, 0, 0))

	{

		TranslateMessage(&msg);

		DispatchMessage(&msg);

	}



	return 0;

}



LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)

{

	switch (msg)

	{

	case WM_COMMAND:

		switch (LOWORD(wParam))

		{

		case IDC_MAIN_BUTTON:

		{

			char szBuffer[1024];

			ZeroMemory(szBuffer, sizeof(szBuffer));



			SendMessage(hEditOut,

				WM_GETTEXT,

				sizeof(szBuffer),

				reinterpret_cast<LPARAM>(szBuffer));



			send(Socket, szBuffer, strlen(szBuffer), 0);



			SendMessage(hEditOut, WM_SETTEXT, 0, (LPARAM)"");

		}

		break;

		}

		break;

	case WM_CREATE:

	{

		ZeroMemory(szHistory, sizeof(szHistory));



		// Create incoming message box

		hEditIn = CreateWindowEx(WS_EX_CLIENTEDGE,

			"EDIT",

			"",

			WS_CHILD | WS_VISIBLE | ES_MULTILINE |

			ES_AUTOVSCROLL | ES_AUTOHSCROLL,

			50,

			120,

			400,

			200,

			hWnd,

			(HMENU)IDC_EDIT_IN,

			GetModuleHandle(NULL),

			NULL);

		if (!hEditIn)

		{

			MessageBox(hWnd,

				"Could not create incoming edit box.",

				"Error",

				MB_OK | MB_ICONERROR);

		}

		HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);

		SendMessage(hEditIn,

			WM_SETFONT,

			(WPARAM)hfDefault,

			MAKELPARAM(FALSE, 0));

		SendMessage(hEditIn,

			WM_SETTEXT,

			0,

			(LPARAM)"Waiting for client to connect...");



		// Create outgoing message box

		hEditOut = CreateWindowEx(WS_EX_CLIENTEDGE,

			"EDIT",

			"",

			WS_CHILD | WS_VISIBLE | ES_MULTILINE |

			ES_AUTOVSCROLL | ES_AUTOHSCROLL,

			50,

			50,

			400,

			60,

			hWnd,

			(HMENU)IDC_EDIT_IN,

			GetModuleHandle(NULL),

			NULL);

		if (!hEditOut)

		{

			MessageBox(hWnd,

				"Could not create outgoing edit box.",

				"Error",

				MB_OK | MB_ICONERROR);

		}



		SendMessage(hEditOut,

			WM_SETFONT,

			(WPARAM)hfDefault,

			MAKELPARAM(FALSE, 0));

		SendMessage(hEditOut,

			WM_SETTEXT,

			0,

			(LPARAM)"Type message here...");



		// Create a push button

		HWND hWndButton = CreateWindow(

			"BUTTON",

			"Send",

			WS_TABSTOP | WS_VISIBLE |

			WS_CHILD | BS_DEFPUSHBUTTON,

			50,

			330,

			75,

			23,

			hWnd,

			(HMENU)IDC_MAIN_BUTTON,

			GetModuleHandle(NULL),

			NULL);



		SendMessage(hWndButton,

			WM_SETFONT,

			(WPARAM)hfDefault,

			MAKELPARAM(FALSE, 0));



		WSADATA WsaDat;

		int nResult = WSAStartup(MAKEWORD(2, 2), &WsaDat);

		if (nResult != 0)

		{

			MessageBox(hWnd,

				"Winsock initialization failed",

				"Critical Error",

				MB_ICONERROR);

			SendMessage(hWnd, WM_DESTROY, 0, 0);

			break;

		}



		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (Socket == INVALID_SOCKET)

		{

			MessageBox(hWnd,

				"Socket creation failed",

				"Critical Error",

				MB_ICONERROR);

			SendMessage(hWnd, WM_DESTROY, 0, 0);

			break;

		}



		SOCKADDR_IN SockAddr;

		SockAddr.sin_port = htons(nPort);

		SockAddr.sin_family = AF_INET;

		SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);



		if (bind(Socket, (LPSOCKADDR)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR)

		{

			MessageBox(hWnd, "Unable to bind socket", "Error", MB_OK);

			SendMessage(hWnd, WM_DESTROY, 0, 0);

			break;

		}



		nResult = WSAAsyncSelect(Socket,

			hWnd,

			WM_SOCKET,

			(FD_CLOSE | FD_ACCEPT | FD_READ));

		if (nResult)

		{

			MessageBox(hWnd,

				"WSAAsyncSelect failed",

				"Critical Error",

				MB_ICONERROR);

			SendMessage(hWnd, WM_DESTROY, 0, 0);

			break;

		}



		if (listen(Socket, (1)) == SOCKET_ERROR)

		{

			MessageBox(hWnd,

				"Unable to listen!",

				"Error",

				MB_OK);

			SendMessage(hWnd, WM_DESTROY, 0, 0);

			break;

		}

	}

	break;



	case WM_DESTROY:

	{

		PostQuitMessage(0);

		shutdown(Socket, SD_BOTH);

		closesocket(Socket);

		WSACleanup();

		return 0;

	}

	break;



	case WM_SOCKET:

	{

		switch (WSAGETSELECTEVENT(lParam))

		{

		case FD_READ:

		{

			char szIncoming[1024];

			ZeroMemory(szIncoming, sizeof(szIncoming));



			int inDataLength = recv(Socket,

				(char*)szIncoming,

				sizeof(szIncoming) / sizeof(szIncoming[0]),

				0);



			strncat(szHistory, szIncoming, inDataLength);

			strcat(szHistory, "\r\n");



			SendMessage(hEditIn,

				WM_SETTEXT,

				sizeof(szIncoming) - 1,

				reinterpret_cast<LPARAM>(&szHistory));

		}

		break;



		case FD_CLOSE:

		{

			MessageBox(hWnd,

				"Client closed connection",

				"Connection closed!",

				MB_ICONINFORMATION | MB_OK);

			closesocket(Socket);

			SendMessage(hWnd, WM_DESTROY, 0, 0);

		}

		break;



		case FD_ACCEPT:

		{

			int size = sizeof(sockaddr);

			Socket = accept(wParam, &sockAddrClient, &size);

			if (Socket == INVALID_SOCKET)

			{

				int nret = WSAGetLastError();

				WSACleanup();

			}

			SendMessage(hEditIn,

				WM_SETTEXT,

				0,

				(LPARAM)"Client connected!");

		}

		break;

		}

	}

	}



	return DefWindowProc(hWnd, msg, wParam, lParam);

}