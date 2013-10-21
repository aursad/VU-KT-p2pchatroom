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
    if ( (UserInput [0] == 'S' || UserInput [0] == 's') &&
            (UserInput [1] == 'E' || UserInput [1] == 'e') &&
            (UserInput [2] == 'T' || UserInput [2] == 't') &&
            (UserInput [3] == 'N' || UserInput [3] == 'n') &&
            (UserInput [4] == ' ') )
    {
        strcpy (Packet, UserInput);
        MarshalPacket (Packet);
        strcpy (UserInput, Packet);
        return 1;
    }
    if ( (UserInput [0] == 'J' || UserInput [0] == 'j') &&
            (UserInput [1] == 'O' || UserInput [1] == 'o') &&
            (UserInput [2] == 'I' || UserInput [2] == 'i') &&
            (UserInput [3] == 'N' || UserInput [3] == 'n') &&
            (UserInput [4] == ' ') )
    {
        strcpy (Packet, UserInput);
        MarshalPacket (Packet);
        strcpy (UserInput, Packet);
        return 1;
    }
        if ( (UserInput [0] == 'R' || UserInput [0] == 'r') &&
            (UserInput [1] == 'O' || UserInput [1] == 'o') &&
            (UserInput [2] == 'O' || UserInput [2] == 'o') &&
            (UserInput [3] == 'M' || UserInput [3] == 'm') &&
            (UserInput [4] == 'S' || UserInput [4] == 's') )
    {
        strcpy (Packet, UserInput);
        MarshalPacket (Packet);
        strcpy (UserInput, Packet);
        return 1;
    }
    /*
    Prisijungimas prie kanalo
    HELP
     */
    if ( (UserInput [0] == 'H' || UserInput [0] == 'h') &&
            (UserInput [1] == 'E' || UserInput [1] == 'e') &&
            (UserInput [2] == 'L' || UserInput [2] == 'l') &&
            (UserInput [3] == 'P' || UserInput [3] == 'p') )
    {
        printf("\n------\n");
        printf("HELP CHAT ROOM\n\n");
        printf("Galimos komandos sistemoje:\n");
        printf("setn vardas - nustatyti savo varda sistemoje\n");
        printf("join id     - kanalo numeris prie kurio norima prisijungti\n");
        printf("rooms       - rodyti visu kanalu informacija\n");
        printf("------\n\n");
        return 3;
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
