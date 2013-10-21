//Ausra Sakalauskaite, PS'5

#include	"cnlabs-server3.h"
struct s_user *client;

SOCKET InitializeServer ( void )
{
 	//Inicializavimas; Lokaliu kintamuju deklaravimas
    SOCKET ServerSockDesc;
	char cServerHostName [256] = {0};          // Hosto, kuriame serveris paleistas, vardas.
    struct sockaddr_in ServerAddress;          // Serverio adreso struktura.
    struct hostent *ptrServerHostEntry = NULL; // Serverio hosto informacine struktura.

    const char yes = '1';



 		//Suzinomas host arba masinos vardas, kuriame paleistas serveris.
        //Naudojama SOCKET API funkcija gethostname(serverioHostoVardas, tipoDydisBaitais).
        if (INVALID_SOCKET == gethostname(cServerHostName, sizeof (cServerHostName))) {
            printf("Klaida : %d", WSAGetLastError());
            return INVALID_SOCKET; //grazina 1
        }

        //Pagal masinos varda suzinome hosto informacija (adresus, protokolus, versijas, ...)
        //naudojama f-ja gethostbyname(serverioHostoVardas).
        if (NULL == (ptrServerHostEntry = gethostbyname(cServerHostName))) {
            printf("Klaida : %d", WSAGetLastError());
            return INVALID_SOCKET; //grazina 1
        }

        ServerAddress.sin_family = AF_INET; //Protokolu seima (IPv4).
        // nepamirstame, kad tinkle galioja big-endian baitu sekos tvarka (nesukeiciam baitu tvarkos).
        ServerAddress.sin_port = htons(SERVER_PORT); // nurodomas hostas
        ServerAddress.sin_addr = *(struct in_addr *) ptrServerHostEntry->h_addr; // nurodomas adresas
        memset(&(ServerAddress.sin_zero), 0, 8); //likusius 8 baitus uzpildo nuliais

        //sukuria serverio soketo deskriptoriu, ir jei negali inicializuoti, tai nutraukia vykdyma
        if (INVALID_SOCKET == (ServerSockDesc = socket(AF_INET, SOCK_STREAM, 0))) //AF_INET -protokolo seima IPv4,
            //SOCK_STREAM - i susijungima orientuotas TCP
            //0-protokolo apribojimai
        {
            printf("Klaida : %d", WSAGetLastError());
            return INVALID_SOCKET; //grazina 1
        }

        //Soketo adreso ir porto pakartotinio panaudojimo pozymis
        //Soketas, kuriam reikalingas sis nustatymas (serverio atveju - klientas)
        //sio parametro lygis
        //Parametro pavadinimas
        //Parametro reiksme—
        //Parametro dydis
        //ServerSockDesc- identifikuoja deskriptoriu, SOL_SOCKET-auksciausias soketo lygis, SO_REUSEADDR-. jei sizeof bus0, nenaudos RESUSEADDR

        if (SOCKET_ERROR == setsockopt(ServerSockDesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes))) {
            printf("Klaida : %d", WSAGetLastError());
            return INVALID_SOCKET; //grazina 1
        }


        //Deskriptoriaus surysimas su adresu
        if (SOCKET_ERROR == bind(ServerSockDesc, (struct sockaddr *) &ServerAddress, sizeof (ServerAddress))) {
            printf("Klaida : %d", WSAGetLastError());
			closesocket(ServerSockDesc);
            return INVALID_SOCKET;
        }
        //patalpina soketa i lauko busena, kurioje galimas prisijungimas
        if (SOCKET_ERROR == listen(ServerSockDesc, MAX_QUEUE_LENGTH)) {
            printf("Klaida : %d", WSAGetLastError());
            closesocket(ServerSockDesc);
            return INVALID_SOCKET;
        }
        printf ("Server: started successfully on host \'%s\' - (%s).\n", cServerHostName, inet_ntoa (*(struct in_addr *)ptrServerHostEntry->h_addr) );
        create_rooms();

	return ServerSockDesc;
}



 int HandleNewConnection (SOCKET *ServerSockDesc, unsigned int *MaxKnownSocketDesc, fd_set *MainSocketSet) {

    struct sockaddr_in RemoteAddress;   // Struktura bandancio prisijungti adresui.
    SOCKET NewConnectionDesc;           // Soketo deskriptorius bendravimui su nauju klientu.
    int AddressSize;                    // Adreso strukturos dydziui saugoti.
    char Buffer [1000] = {0};           // Buferis pasveikinimo informacijai laikyti.
    struct hostent *HostEntry;          // Struktura informacijai apie naujo kliento masina.
    const char yes = '1';

    AddressSize = sizeof (struct sockaddr_in); // Inicializuoti adreso struktûros dydþio kintamàjá.

    //priimamas naujo soketo prisijungimas ir sukuria soketo deskriptoriu bendravimui su nauju klientu.
    if ( SOCKET_ERROR == (NewConnectionDesc =   accept ((*ServerSockDesc), (struct sockaddr *)&RemoteAddress, &AddressSize)) )
    return SOCKET_ERROR;

    //uzdeda pozymi soketo adreso ir porto pakartotiniam panaudojimui nauja klienta identifikuojanciam soketui
    if ( SOCKET_ERROR == setsockopt (NewConnectionDesc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) )
    return SOCKET_ERROR;

    //isveda pranesima apie nauja prisijungima serveryje ir matom is kur prisijunge klientas
    //inet_ntoa- konvertuoja adresa i ASCII koda
    printf ("Server: new connection from \'%s\' accepted successfully.\n", inet_ntoa (RemoteAddress.sin_addr));
    client = client_create("Klientas", NewConnectionDesc);
    //first_login(NewConnectionDesc);
    //gaunama info apie klienta, jei neiseina, tai uzdarom naujai sukurta soketa
    if ( NULL == (HostEntry = gethostbyaddr ((void*)&(RemoteAddress.sin_addr), sizeof (RemoteAddress.sin_addr), AF_INET)) )
	{
       closesocket (NewConnectionDesc);
       printf ("Server: new connection from \'%s\' was immediately closed because of gethostbyaddr() failure.\n");
       client_destroy(client);
       return SOCKET_ERROR;
    }

    //Talpinamas pasveikinimas, tam iskirtoje vietoje
    strcpy (Buffer, "\n/*****************************************************************************\\\n");
    strcat (Buffer, " Dear guest from \'");
    strcat (Buffer, HostEntry->h_name);
    strcat (Buffer, "\', we welcome you at CN Labs Server!\n\n");
    strcat (Buffer, "\\*****************************************************************************/\n\n");


    //Suformuoti paketa, paruosti duomenu persiuntimui, meta duomenu pridejimas
    MarshalPacket (Buffer);

  //pasiunciamas pasveikinimas naujai prisijungusiam
    if ( SOCKET_ERROR == SendPacket (&NewConnectionDesc, Buffer, strlen (Buffer)) ){
        closesocket (NewConnectionDesc);
        printf ("Server: new connection from \'%s\' was immediately closed because of send() failure.\n");
        client_destroy(client);
        return SOCKET_ERROR;
   }

    //sukurtas sokettu deskriptorius itraukiamas i pagrindiniu soketu aibe
    FD_SET (NewConnectionDesc, MainSocketSet);

    //modifikuojamas max deskriptoriaus kintamasis
    if ( (*MaxKnownSocketDesc) < NewConnectionDesc )
    (*MaxKnownSocketDesc) = NewConnectionDesc;

    //sekmes reiksme, kviecianciajai programai
    return 1;
}



 void HandleDataFromClient (SOCKET ClientSockDesc, fd_set *MainSocketSet){

int BytesOfPacketReceived = 0;  // Gautu vieno paketo baitu skaicius.
char *ClientDataBuffers [10];   // Buferis kliento pasiustai informacijai saugoti.
int ReceiveResult;              // Duomenu gavimo funkcijos grazinamam rez. saugoti.
int PacketsQuantity = 0;        // Duomenu gavimo operacija gautu paketu skaicius.
int iCounter;                   // Skaitliukas.
char Command [2000] = {0};      // Komandai saugoti.
int SendResult;
    unsigned int ParseResult;    // Komandu analizes rezultatui saugoti.
//bandoma gauti i isskirta buferi kliento issiusta informacija
ReceiveResult = ReceivePacket (&ClientSockDesc, Command);

//atliekam veiksmus pagal gavimo funkcijos pranesta rezultata. Jei gautas pranesimas, kad rysys nutrauktas -isveda pranesima
if ( ReceiveResult == 0 ){
    printf ("Server: client \'%s\' has quit the connection.\n", client->name);
    client_destroy(client);
    closesocket (ClientSockDesc);
    FD_CLR (ClientSockDesc, MainSocketSet);
}

//atliekam veiksmus pagal duoemnu gavimo funkcijos pranesta rezultata, jei klaida skaitant duomenis-pranesimas ir uzdaromas soketas
if ( ReceiveResult == SOCKET_ERROR ){
    printf ("Server error: data reception from client was erroneous at socket %d.\n", ClientSockDesc);
    // Dabar reikia surast ir pasalinti visa informacija susijusia su kliento aptarnavimu, atlaisvinti klientui skirtus resursus.
    closesocket (ClientSockDesc);
    FD_CLR (ClientSockDesc, MainSocketSet);
}

//Jei viska pavyko nuskaityti sekmingai gerai, tai apdorojame kliento uzklausa.
if ( ReceiveResult == 1 ){
        UnmarshalPacket(Command);
        Command [strlen (Command)] = '\0';
        ParseResult = ParseCommandInput (Command, client);

        if ( 1 == ParseResult )
        {
          if (SOCKET_ERROR == SendPacket(&ClientSockDesc, Command, strlen(Command))) {
                printf("CNLabs Client error: data transmission to the server failed.\n");
            }
        }

     printf ("Server: data reception from client was successful at socket %d.\n", ClientSockDesc);
}
 }




