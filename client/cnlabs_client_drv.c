#include "cnlabs_client.h"
#define BUFFLEN 1024
int main(void)
{
    SOCKET ClientSockDesc;       // Kliento pagrindinio soketo-klausytojo deskriptorius.
    char UserInput [BUFFLEN] = {0}; // Masyvas vartotojo komandoms nuskaityti.
    int SendResult;              // Siuntimo funkcijos resultatui saugoti.
    int ReceiveResult;              // Siuntimo funkcijos resultatui saugoti.
    char Packet [BUFFLEN] = {0};    // Buferiu masyvas duomenims gauti.
    int jCounter, i;      // Skaitliukai.
    unsigned int ParseResult;    // Komandu analizes rezultatui saugoti.

    fd_set MainSocketSet, TempSet; //pagrindine soketu aibe; soketu aibe, kuri turi duomenu, paruostu nuskaitymui
    unsigned int MaxKnownSocketDesc, iCounter;// maksimalus deskriptoriu kintamasis (soketo nr), skaitliukas (soketè)
    struct timeval TimeVal; // Laiko struktura dirbti su select().

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
    // Inicializuoti soketu aibes.
    FD_ZERO(&MainSocketSet);
    FD_ZERO(&TempSet);

    // Inicializuojame laiko struktura. 0 - reiskia, kad select() funkcija turi blokuotis laukdama, kol atsiras bent vienas aktyvus soketas.
    TimeVal.tv_sec = 10; //sekundes
    TimeVal.tv_usec = 500; //tukstantosios

    FD_SET(ClientSockDesc, &MainSocketSet);
    MaxKnownSocketDesc = ClientSockDesc;

    while ( 1 )
    {
        // Kiekvienoje iteracijoje inicializuokime pagalbine soketu aibe pagrindine.
        TempSet = MainSocketSet;

        // Isrenkame is soketu aibes tuos deskriptorius, kurie apraso soketus, turincius duomenu, paruostu skaitymui.
        //select()tinklo primityvo panaudojimas sinchroniniu budu, kada blokuojamasi laukiant aktyviu soketu, turinciu info, kuria per juos galima skaityti is tinklo.
        //tikrina zemesnius uz pirmuoji nr pazymetus, todel +1, pirmos eiles soketu aibe (2,3 eiles soketu aibe), blokavimas ribota laika
        if ( SOCKET_ERROR == select (MaxKnownSocketDesc + 1, &TempSet, NULL, NULL, &TimeVal) )
        {
            exit(EXIT_FAILURE);
        }

        for ( iCounter = 0; iCounter <= MaxKnownSocketDesc; iCounter++ )
        {
            if (FD_ISSET(iCounter, &TempSet))
            {
                if ( SOCKET_ERROR == ReceivePacket (&ClientSockDesc, Packet) )
                {
                    closesocket (ClientSockDesc);
                    return INVALID_SOCKET;
                }
                UnmarshalPacket (Packet);
                printf ("%s\n", Packet);
            }
            else if(FD_ISSET(0, &TempSet))
            {
                //fflush ( stdin );
                memset (UserInput, 0, sizeof (UserInput));
                fgets (UserInput, sizeof (UserInput), stdin);
                UserInput [strlen (UserInput) - 1] = '\0';
                //ParseCommandInput (UserInput);
                MarshalPacket(UserInput);
                SendPacket (&ClientSockDesc, UserInput,strlen(UserInput));
            }

        }
    }
    closesocket (ClientSockDesc);
EXIT:
    WSACleanup();
    return 1;
}
