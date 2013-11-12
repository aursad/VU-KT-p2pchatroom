#ifndef CNLABS_SERVER3_H //neleidzia apibrezti 2 header failu
#define	CNLABS_SERVER3_H

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>    // 1. Itraukiame pagrindini winapi '.h' faila.
#include <winsock2.h>   // 2. Itraukiame darbui su soketu api skirta '.h' faila.
#include <time.h>       // 3. Itraukiame darbui su laiku skirta '.h' faila.

#include "cnlabs-common3.h"

//Funkciju aprasai-------------------------------------------------------------
//Funkciju, inicializuojanciu serverio rutinas, antrastes.
SOCKET InitializeServer(void);
//Naujo kliento prisijungimo apdorojimo funkcija
int HandleNewConnection (SOCKET *ServerSockDesc, unsigned int *MaxKnownSocketDesc, fd_set *MainSocketSet);
//esamo kliento uzklausos apdorojimas
void HandleDataFromClient (SOCKET ClientSockDesc, fd_set *MainSocketSet);


#endif	/* HEADER_H */


