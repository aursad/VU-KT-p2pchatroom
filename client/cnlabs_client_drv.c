#include "cnlabs_client.h"
int main(void)
{
    SOCKET ClientSockDesc;       // Kliento pagrindinio soketo-klausytojo deskriptorius.
    char UserInput [2000] = {0}; // Masyvas vartotojo komandoms nuskaityti.
    int SendResult;              // Siuntimo funkcijos resultatui saugoti.
    int ReceiveResult;              // Siuntimo funkcijos resultatui saugoti.
    char Packet [2000] = {0};    // Buferiu masyvas duomenims gauti.
    int iCounter, jCounter;      // Skaitliukai.
    unsigned int ParseResult;    // Komandu analizes rezultatui saugoti.

        fd_set MainSocketSet, TempSet; //pagrindine soketu aibe; soketu aibe, kuri turi duomenu, paruostu nuskaitymui
    unsigned int MaxKnownSocketDesc;// maksimalus deskriptoriu kintamasis (soketo nr), skaitliukas (soket¸)
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
    TimeVal.tv_sec = 0; //sekundes
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

        // Nuskanuoti galimu deskriptoriu erdve iki turimo maksimalaus.
        for ( iCounter = 0; iCounter <= MaxKnownSocketDesc; iCounter++ )
        {
            // Tikriname su kiekviena skaitliuko reiksme, ar jo aprasomas soketas nepriklauso pagrindinei soketu aibei.
            if ( FD_ISSET (iCounter, &TempSet) )
            {
                // Jei tos skaitliuko reiksmes aprasomas soketas yra 'ServerSockDesc', t.y. nauju prisijungimu laukiantis, tai reikia apdoroti naujo kliento prisijungima.
                //if ( iCounter == ClientSockDesc )
                //{
                    if ( SOCKET_ERROR == ReceivePacket (&ClientSockDesc, Packet) )
                    {
                        printf ("CNLabs Client error: data reception from server failed.\n");
                        break;
                    }
                    UnmarshalPacket (Packet);
                    printf ("%s\n", Packet);
               //}
            } else {
                fflush ( stdin );
                memset (UserInput, 0, sizeof (UserInput));
                fgets (UserInput, sizeof (UserInput), stdin);
                UserInput [strlen (UserInput) - 1] = '\0';
                if ( 0 == strcmp (UserInput, "") ) continue;
                ParseResult = ParseCommandInput (UserInput);
                if ( 0 == ParseResult ) continue;
                if ( 3 == ParseResult ) continue;
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
                    UnmarshalPacket (Packet);
                    printf ("%s\n", Packet);
                }
                if ( 2 == ParseResult ) break;
            }

        }
    }
    closesocket (ClientSockDesc);
EXIT:
    WSACleanup();
    return 1;
}
