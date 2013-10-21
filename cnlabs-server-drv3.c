////Ausra Sakalauskaite, PS'5

#include "cnlabs-server3.h"
int main(void)
{
    SOCKET ServerSockDesc;
    
    fd_set MainSocketSet, TempSet; //pagrindine soketu aibe; soketu aibe, kuri turi duomenu, paruostu nuskaitymui
    unsigned int MaxKnownSocketDesc, iCounter;// maksimalus deskriptoriu kintamasis (soketo nr), skaitliukas (soketø)
    struct timeval TimeVal; // Laiko struktura dirbti su select().

    WSADATA wsaData;
    WSAStartup(MAKEWORD(1, 1), &wsaData);


    // Inicializuoti soketu aibes.
    FD_ZERO(&MainSocketSet);
    FD_ZERO(&TempSet);
 
    // Inicializuojame laiko struktura. 0 - reiskia, kad select() funkcija turi blokuotis laukdama, kol atsiras bent vienas aktyvus soketas.
    TimeVal.tv_sec = 0; //sekundes
    TimeVal.tv_usec = 0; //tukstantosios
 
    // Inicializuokime serverio funkcini moduli.
    if ( INVALID_SOCKET == (ServerSockDesc = InitializeServer()) )
    {
        exit(EXIT_FAILURE);
    }
   
    // Itraukti serverio klausanti soketa i pagrindine aibe ir pazymeti klausantiji soketa kaip maksimalu programai zinoma soketo deskriptoriu.
    FD_SET(ServerSockDesc, &MainSocketSet);
    MaxKnownSocketDesc = ServerSockDesc;
 
    // Pagrindinis amzinas ciklas, kuris palaiko serveri rezimu "gyvas".
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
                if ( iCounter == ServerSockDesc )
                {
                    // Apdorojame nauja prisijungima.
                    //(serverio soketo adresas, rodykle i max soketo nr, rodykle i pagrindine soketu aibe)
                    if ( SOCKET_ERROR == HandleNewConnection (&ServerSockDesc, &MaxKnownSocketDesc, &MainSocketSet) )
                        printf ("Server error: acceptance of new connection was erroneous.\n");
                }
 
                // Jei tai ne nauju prisijungimu laukiancio soketo deskriptorius, tai reiskia, kad gauta informacija is kazkurio prisijungusio kliento.
                // Reikia ja priimti ir atitinkamai apdoroti.
                else
                {
                    // Priimame ir apdorojame pranesima.
                    //(kliento soketo deskriptoriaus nr, rodykle i pagrindine soketu aibe
                    HandleDataFromClient ((SOCKET)iCounter, &MainSocketSet);
                }
            }
        }
    }

    WSACleanup();
    
        return 0;
}
