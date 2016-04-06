// VRPN_Server.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>
#include <vrpn_Tracker.h>

#pragma comment(lib,"ws2_32.lib")

#define BUFLEN 512
#define PORT 8888

void VRPN_CALLBACK trackerHandler(void *userdata, const vrpn_TRACKERCB t);
void sendBuffer();
void setupUDPSocket();

SOCKET s;
struct sockaddr_in server, si_other;
int slen, recv_len;
char sBuffer[BUFLEN];
char rBuffer[BUFLEN];
WSADATA wsa;

float test[7] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Too few arguments\n");
		return 1;
	}

	if (argc > 2)
	{
		printf("Too many arguments\n");
		return 1;
	}

	printf("Tracker: %s\n", argv[1]);

	vrpn_Tracker_Remote tracker(argv[1]);
	tracker.register_change_handler(NULL, trackerHandler);

	setupUDPSocket();

	while(true)
	{
		tracker.mainloop();
		sendBuffer();
	}

	closesocket(s);
	WSACleanup();

	return 0;
}

void VRPN_CALLBACK trackerHandler(void *userdata, const vrpn_TRACKERCB t)
{
	float msg[7];

	//float64 to float
	msg[0] = t.pos[0];
	msg[1] = t.pos[1];
	msg[2] = t.pos[2];

	msg[3] = t.quat[0];
	msg[4] = t.quat[1];
	msg[5] = t.quat[2];
	msg[6] = t.quat[3];

	//printf("Position: %f %f %f\n", msg[0], msg[1], msg[2]);
	//printf("Rotation: %f %f %f %f\n", msg[3], msg[4], msg[5], msg[6]);

	memset(sBuffer, '\0', BUFLEN);

	memcpy(sBuffer, msg, 7*sizeof(float));
}

void sendBuffer()
{
	//printf("Waiting for data...");

	//clear the buffer by filling null, it might have previously received data
	memset(rBuffer, '\0', BUFLEN);

	//try to receive some data, this is a blocking call
	if ((recv_len = recvfrom(s, rBuffer, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//print details of the client/peer and the data received
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %s\n", rBuffer);

	if (sendto(s, sBuffer, 7*sizeof(float), 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}

void setupUDPSocket()
{
	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

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
	printf("Bind done\n");
	memset(sBuffer, '\0', BUFLEN);
}