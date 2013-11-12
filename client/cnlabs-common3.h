#ifndef CNLABS_COMMON3_H
#define	CNLABS_COMMON3_H

#include <windows.h>    // 1. Itraukiame pagrindini winapi '.h' faila.
#include <winsock2.h>   // 2. Itraukiame darbui su soketu api skirta '.h' faila.
#include <time.h>       // 3. Itraukiame darbui su laiku skirta '.h' faila.

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

//Konstantos.
#define SERVER_PORT 1000
#define MAX_QUEUE_LENGTH 5

void MarshalPacket ( char* Packet );
int UnmarshalPacket ( char* Packet );
int SendPacket ( SOCKET* s, const char* Packet, int MaxBufferSize);
int ReceivePacket ( SOCKET* s, char* Packet);

#endif
