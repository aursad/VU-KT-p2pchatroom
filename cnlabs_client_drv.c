#include "cnlabs_client.h"
int main(void)
{
    SOCKET ClientSockDesc;       // Kliento pagrindinio soketo-klausytojo deskriptorius.
    char UserInput [2000] = {0}; // Masyvas vartotojo komandoms nuskaityti.
    int SendResult;              // Siuntimo funkcijos resultatui saugoti.
    int ReceiveResult;              // Siuntimo funkcijos resultatui saugoti.
    char *Packet [2000] = {0};    // Buferiu masyvas duomenims gauti.
    int iCounter, jCounter;      // Skaitliukai.
    unsigned int ParseResult;    // Komandu analizes rezultatui saugoti.

WSADATA wsaData;
if(WSAStartup(0x202, &wsaData) == 0) {
}else {printf("ERROR: Initialization failure.\n");}

    if ( INVALID_SOCKET == (ClientSockDesc = InitializeClient ()) )
    {
        printf ("CNLabs Client error: client initialization failed.\n");
        goto EXIT;
    }
    while ( 1 )
    {
        fflush ( stdin );
        memset (UserInput, 0, sizeof (UserInput));
        fgets (UserInput, sizeof (UserInput), stdin);
        UserInput [strlen (UserInput) - 1] = '\0';
        if ( 0 == strcmp (UserInput, "") ) continue;
        ParseResult = ParseCommandInput (UserInput);
        if ( 0 == ParseResult ) continue;
        if ( 1 == ParseResult )
{
            if ( SOCKET_ERROR == (SendResult = SendPacket (&ClientSockDesc, UserInput, strlen (UserInput))) )
            {
                printf ("CNLabs Client error: data transmission to the server failed.\n");
                break;
            }
            if ( SOCKET_ERROR == ReceivePacket (&ClientSockDesc, Packet) )
            {
                printf ("CNLabs Client error: data reception from server failed.\n");
                break;
            }
            //strcpy (Packet, UserInput);
            printf ("    ASNWER PACKET  : \'%s\'\n", Packet);
            UnmarshalPacket (Packet);
            printf ("    ANSWER         : \'%s\'\n", Packet);
        }
        if ( 2 == ParseResult ) break;
    }
    closesocket (ClientSockDesc);
EXIT:
    WSACleanup();
    return 1;
}
