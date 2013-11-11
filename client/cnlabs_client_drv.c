#include "cnlabs_client.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#define BUFFLEN 1024

int main(void)
{
  SOCKET ClientSockDesc;       // Kliento pagrindinio soketo-klausytojo deskriptorius.
    char UserInput [2000] = {0}; // Masyvas vartotojo komandoms nuskaityti.
    int SendResult;              // Siuntimo funkcijos resultatui saugoti.
    int ReceiveResult;              // Siuntimo funkcijos resultatui saugoti.
    char Packet [2000] = {0};    // Buferiu masyvas duomenims gauti.
    int iCounter, jCounter;      // Skaitliukai.
    unsigned int ParseResult;    // Komandu analizes rezultatui saugoti.

    fd_set read_set; //pagrindine soketu aibe; soketu aibe, kuri turi duomenu, paruostu nuskaitymui
    char recvbuffer[BUFFLEN];
    char sendbuffer[BUFFLEN];
     int s_socket, i;

    WSADATA wsaData;
    if(WSAStartup(0x202, &wsaData) == 0)
    {
    }
    else
    {
        printf("ERROR: Initialization failure.\n");
    }

    if ( INVALID_SOCKET == (ClientSockDesc = InitializeClient ()) )
    {
        printf ("CNLabs Client error: client initialization failed.\n");
        goto EXIT;
    }

    memset(&sendbuffer,0,BUFFLEN);
    //fcntl(0,F_SETFL,fcntl(0,F_GETFL,0)|O_NONBLOCK);
    //fseek(ClientSockDesc, 0L, SEEK_END);
   u_long r = 1;
    iCounter = 0;
   if( ioctlsocket(0, FIONBIO, &r ) == SOCKET_ERROR )
   {
      perror( "imc_connect: ioctlsocket failed" );
   }
    while ( 1 )
    {
        FD_ZERO(&read_set);
        FD_SET(ClientSockDesc,&read_set);
        FD_SET(0,&read_set);
        s_socket = ClientSockDesc;
        select(iCounter+1,&read_set,NULL,NULL,NULL);

        if (FD_ISSET(iCounter, &read_set)){
            memset(&recvbuffer,0,BUFFLEN);
            i = read(iCounter, &recvbuffer, BUFFLEN);
            //i = recv(s_socket,&recvbuffer,BUFFLEN,0);
            printf("%s \n",recvbuffer);
            printf("i: %d\n", i);
            iCounter++;
        }
        else if (FD_ISSET(0,&read_set)) {
            i = read(0,&sendbuffer,1);
            //ParseCommandInput (sendbuffer);
            //MarshalPacket(recvbuffer);
            SendPacket (&ClientSockDesc, sendbuffer,i);
        }
/*
(SendResult = SendPacket (&ClientSockDesc, UserInput, strlen (UserInput))
ReceivePacket (&ClientSockDesc, Packet)
UnmarshalPacket (Packet);
printf ("%s\n", Packet);

                    */
    }
    closesocket (ClientSockDesc);
EXIT:
    WSACleanup();
    return 1;
}
