// VRPN_Server.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>
#include <vrpn_Tracker.h>

#pragma comment(lib,"ws2_32.lib")

#define BUFLEN 512
#define PORT 8888

void VRPN_CALLBACK trackerHandler(void *userdata, const vrpn_TRACKERCB t);

SOCKET s;
struct sockaddr_in server, si_other;
int slen, recv_len;
char buf[BUFLEN];
WSADATA wsa;


int main(int argc, char* argv[])
{
	printf("Tracker: %s Client: %s\n", argv[1], argv[2]);

	if (argc < 2)
	{
		printf("Too few arguments\n");
		return 1;
	}

	if (argc > 3)
	{
		printf("Too many arguments\n");
		return 1;
	}

	vrpn_Tracker_Remote tracker(argv[1]);
	tracker.register_change_handler(NULL, trackerHandler);

	//setup socket
	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	while(true)
	{
		tracker.mainloop();
	}

	closesocket(s);
	WSACleanup();

	return 0;
}

void VRPN_CALLBACK trackerHandler(void *userdata, const vrpn_TRACKERCB t)
{
	memset(buf, '\0', BUFLEN);
	memcpy(&buf[0], &t.pos[0], 3 * sizeof(float));
	memcpy(&buf[3 * sizeof(float)], &t.quat[0], 4 * sizeof(float));

	if (sendto(s, buf, 7 * sizeof(float), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}