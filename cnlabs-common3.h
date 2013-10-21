
#ifndef CNLABS_COMMON3_H
#define	CNLABS_COMMON3_H

#include <windows.h>    // 1. Itraukiame pagrindini winapi '.h' faila.
#include <winsock2.h>   // 2. Itraukiame darbui su soketu api skirta '.h' faila.
#include <time.h>       // 3. Itraukiame darbui su laiku skirta '.h' faila.

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include <assert.h>

//Konstantos.
#define SERVER_PORT 1000
#define MAX_QUEUE_LENGTH 5
#define CHATMEMBERS 5
#define CHATROOMS 5

void MarshalPacket ( char* Packet );
int UnmarshalPacket ( char* Packet );
int SendPacket ( SOCKET* s, const char* Packet, int MaxBufferSize);
int ReceivePacket ( SOCKET* s, char* Packet);

/* STRUKTÛROS */
struct s_user{                     /* the structure type of user */
    char *name;             /* user name */
    int socket;             /* user adress socket */
    int id_channel;                    /* channel  */
};
struct s_room{
    int id;
    char title[20]; // Chatroom name
    char desc[50]; // Room desc
    int online; // Number online people
    int people[5]; // list online people
};
struct s_user user;           /* define the structure */
struct s_room room[CHATROOMS];
/* FUNKCIJOS */

#endif
