#include	"cnlabs_client.h"

SOCKET InitializeClient ( void )
{
    SOCKET ClientSockDesc;                     // Kliento soketo deskriptorius.
    char ServerHostName [256] = {0};           // Hosto, kuriame paleistas serveris, vardas.
    unsigned short int ServerPortNumber;       // Porto numeris, kuriuo serveris laukia klientu.
    struct sockaddr_in ServerAddress;          // Serverio adreso struktura.
    struct hostent *ptrServerHostEntry = NULL; // Serverio hosto informacine struktura.
    char Temp [10] = {0};
#ifdef WIN32OS
    const char yes = '1';
#else
    const int yes = 1;
#endif
    char Packet [2000] = {0};  // Bufferis pasveikinimo duomenims laikyti.

    while ( 0 == strcmp (ServerHostName, "") )
    {
        printf ("Server host name  : ");
        fgets (ServerHostName, sizeof (ServerHostName), stdin);
    }
    ServerHostName [strlen (ServerHostName) - 1] = '\0';

    while ( 0 == strcmp (Temp, "") )
    {
        printf ("Server port number: ");
        fgets (Temp, sizeof (Temp), stdin);
    }
    Temp [strlen (Temp) - 1] = '\0';
    ServerPortNumber = (unsigned short int)atoi (Temp);
    if ( NULL == (ptrServerHostEntry = gethostbyname (ServerHostName)) )
        return INVALID_SOCKET;

    ServerAddress.sin_family = AF_INET; // Nurodoma protokolu seima - TCP/IP.
    ServerAddress.sin_port = htons ( ServerPortNumber ); // Serverio portas, kuri ivede vartotojas. funkcija htons() skaiciaus baitus isdesto tinklo tvarka (host-to-network-short).
    ServerAddress.sin_addr = *(struct in_addr *)ptrServerHostEntry->h_addr; // Pagrindinis hosto IP adresas.
    memset (&(ServerAddress.sin_zero), 0, 8); // Uzpildoma laisva strukturos zona nuliais.

    if ( INVALID_SOCKET == (ClientSockDesc = socket (AF_INET, SOCK_STREAM, 0)) )
        return INVALID_SOCKET;
    if ( SOCKET_ERROR == setsockopt (ClientSockDesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) )
        return INVALID_SOCKET;
    if ( SOCKET_ERROR == connect (ClientSockDesc, (struct sockaddr *)&ServerAddress, sizeof (struct sockaddr)) )
    {
        closesocket (ClientSockDesc);
        return INVALID_SOCKET;
    }
    printf ("CNLabs Client: connected successfully to host \'%s\' - (%s).\n", ServerHostName, inet_ntoa (*(struct in_addr *)ptrServerHostEntry->h_addr) );

    if ( SOCKET_ERROR == ReceivePacket (&ClientSockDesc, Packet) )
    {
        closesocket (ClientSockDesc);
        return INVALID_SOCKET;
    }
    UnmarshalPacket (Packet);
    printf ("%s\n", Packet);
    return ClientSockDesc;
}
int ParseCommandInput ( char* UserInput )
{
    char Packet [2000] = {0}; // Buferis, skirtas saugoti PDU po atpazintos komandos.
    if ( (UserInput [0] == 'E' || UserInput [0] == 'e') &&
            (UserInput [1] == 'C' || UserInput [1] == 'c') &&
            (UserInput [2] == 'H' || UserInput [2] == 'h') &&
            (UserInput [3] == 'O' || UserInput [3] == 'o') &&
            (UserInput [4] == ' ') )
    {
        strcpy (Packet, UserInput + 5);
        printf ("    COMMAND        : ECHO \'%s\'\n", Packet);
        MarshalPacket (Packet);
        printf ("    COMMAND PACKET : \'%s\'\n", Packet);
        strcpy (UserInput, Packet);
        return 1;
    }
    else if ( 0 == strcmp (UserInput, "QUIT") )
    {
        printf ("    COMMAND  : \'%s\'\n", UserInput);
        printf ("    EXITING ...\n");
        return 2;
    }
    else
    {
        printf ("CNLabs Client error: invalid command \'%s\'\n", UserInput);
        return 0;
    }
    return 0;
}
