

#include	"cnlabs-common3.h"

// Suformuoti paketà, paruoðti duomenis persiuntimui tinklu
void MarshalPacket ( char* Packet ){

	int DataSize = strlen(Packet);   // simboliu kiekis
    char PacketSizeStr [10] = {0};   // saugomas duomenu ilgis simboliniu formetu
    char MarshaledData [2000] = {0}; // saugomas duomenu paketas
    int iCounter;                    //skaitliukas

    sprintf(PacketSizeStr, "%d", DataSize);//siunciamu duomenu simbolinis atvaizdavimas

    //sukelia i paketa, jo dydzio baitus
    for ( iCounter = 0; iCounter < strlen(PacketSizeStr); iCounter++ )
    MarshaledData [iCounter] = PacketSizeStr[iCounter];

    //skirtuku atskirti meta-duomenis, nuo siunciamu
    MarshaledData [strlen(PacketSizeStr)] = ':';

    //ikelti siunciamus duomenis i paketa
    for ( iCounter = 0; iCounter < DataSize; iCounter++ )
    MarshaledData [iCounter + (strlen(PacketSizeStr) + 1)] = Packet [iCounter];
    MarshaledData [iCounter + (strlen(PacketSizeStr) + 1)] = '\0';

    //uzbaigto formuot paketo duomenis nukopijuot i duomenu buferi,  grazint vietoj duotu duomenu
    strcpy (Packet, MarshaledData);
}

//Paketo isformavimas, persiustu duomenu paruosimas interpretavimui
int UnmarshalPacket ( char* Packet ){
	char UnmarshaledData [2000] = {0}; // Buferis, skirtas saugoti realius duomenis isskirtus is paketo.
    int iCounter = 0;                  // Skaitliukas.
    char PacketSizeStr [10] = {0};     // Buferis, skirtas saugoti persiustu duomenu dydi simboliniu formatu.
    int DataSize = 0;                  // Persiustu duomenu dydis, kuris bus grazintas.

    //Ispakuoti paketa, nuimam pakeot dydzio baitus ir is ju atsatom persiustu duomenu dydi
    while ( Packet[iCounter] != ':' && iCounter < strlen(Packet) )
    PacketSizeStr[iCounter] = Packet[iCounter++];
    PacketSizeStr[iCounter] = '\0';
    DataSize = atoi(PacketSizeStr); //konvertuoja stringa i integer

    //nuskaito realius persiustus duomenis
    for ( iCounter = strlen(PacketSizeStr) + 1; iCounter < strlen(Packet); iCounter++ )
    if ( Packet[iCounter] != '\r' && Packet[iCounter] != '\n' )
        UnmarshaledData [iCounter - (strlen(PacketSizeStr) + 1)] = Packet[iCounter];
    UnmarshaledData [iCounter - (strlen(PacketSizeStr) + 1)] = '\0';

    //nuskaitytus realius duomenis nukopijuot i PDU buferi
    strcpy (Packet, UnmarshaledData);

    //grazinti realiu duomenu dydi
    return DataSize;
}

//Siøsti paketà tinklu.
int SendPacket ( SOCKET* s, const char* Packet, int MaxBufferSize){
	int SentBytes = 0;             // Jau issiustu baitu skaicius.
    int BytesLeft = MaxBufferSize; // Kiek baitu dar liko issiusti.
    int nBytes;                    // Per viena karta issiunciamu baitu sk.

    //siusti duota paketa buferyje
    while( SentBytes < MaxBufferSize ){
    	//bandoma issiusti dali duomenu
        nBytes = send ((*s), Packet + SentBytes, BytesLeft, 0);

		//Jei klaida nutraukia tikrinima ir pranesa vartotojui
        if ( nBytes == SOCKET_ERROR ){
        	printf("Klaida : %d", WSAGetLastError());
        	return SOCKET_ERROR;
        }

        //modifikuoja issiustu ir dar likusiu baitu skaitliuka
        SentBytes += nBytes;
        BytesLeft -= nBytes;
    }
    //grazina issiustu baitu skaiciu
    return SentBytes;
}

//Gauti paketa tinklu.
int ReceivePacket ( SOCKET* s, char* Packet){
	char DataBuffer [2000] = {0};      // Duomenu buferis.
    int nBytes;                        // Baitu, gautu vienu kreipiniu i recv(), skaicius.

    //kreipiamasi norint gauti dali informacijos
    nBytes = recv ((*s), DataBuffer, sizeof (DataBuffer), 0);
    //Atlikti reikiamus veiksmus tolesniam darbui pagal nBytes reiksmæ. Jei nBytes == 0, tai reiðkia, kad vartotojas nutraukë ryðá su serveriu.
    if ( 0 == nBytes ) return 0;

    //Prieðingu atveju, praneðame, kad skaitant ávyko klaida.
    else if ( SOCKET_ERROR == nBytes ) return SOCKET_ERROR;

    //Prieðingu atveju, mes perskaitëme kaþkokià dalá informacijos. Jà reikia apiforminti taip, kaip to laukia HandleDataFromClient().
    else{
    memset (Packet, 0, nBytes);
    strcpy (Packet, DataBuffer);
    }
    printf("Recv bytes: %d\n", nBytes);
    //grazina sekmes koda
    return 1;
}
int ParseCommandInput ( char* UserInput, int id_socket )
{
    char Packet [2000] = {0}; // Buferis, skirtas saugoti PDU po atpazintos komandos.
    if ( (UserInput [0] == 'S' || UserInput [0] == 's') &&
            (UserInput [1] == 'E' || UserInput [1] == 'e') &&
            (UserInput [2] == 'T' || UserInput [2] == 't') &&
            (UserInput [3] == 'N' || UserInput [3] == 'n') &&
            (UserInput [4] == ' ') )
    {
        int ids;
        ids = search_member(id_socket);
        strcpy (Packet, UserInput+5);
        strcpy (Packet, "System >> ");
//        strcat(Packet, strdup(members[ids].name);

        client_change_name(id_socket, UserInput+5);

        strcat(Packet, " jusu vardas pakeistas i - ");
        strcat(Packet, UserInput+5);
        MarshalPacket (Packet);
        strcpy (UserInput, Packet);
        return 1;
    }
    /*
    Prisijungimas prie kanalo
    ROOMS
     */
    if ( (UserInput [0] == 'O' || UserInput [0] == 'o') &&
            (UserInput [1] == 'N' || UserInput [1] == 'n') &&
            (UserInput [2] == 'L' || UserInput [2] == 'l') &&
            (UserInput [3] == 'I' || UserInput [3] == 'i') &&
            (UserInput [4] == 'N' || UserInput [4] == 'n') &&
            (UserInput [5] == 'E' || UserInput [5] == 'e') )
    {
        strcpy (Packet, "System >> Prisijunge dabar: ");
        strcat(Packet, room[0].online);
        strcat(Packet, " | ");
        int n;
        for(n=0;n<room[0].online;n++)
        {
//            strcat(Packet, members[n].name);
            strcat(Packet, " - ");
        }
        MarshalPacket (Packet);
        strcpy (UserInput, Packet);
        return 1;
    }
    if ( (UserInput [0] == '-' || UserInput [0] == '-') &&
            (UserInput [1] == 'M' || UserInput [1] == 'm') &&
            (UserInput [2] == ' ' || UserInput [2] == ' ') )
    {
        //strcpy (Packet, "System >>");
        //strcat(Packet, UserInput+3);
        int ids;
            ids = search_member(id_socket);
            strcpy(Packet, members[ids].name);
            strcat(Packet, ": ");
            strcat(Packet, UserInput+3);
        MarshalPacket (Packet);
        strcpy (UserInput, Packet);
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
void create_room(char *title, char *desc, int online, int id)
{
    strcpy(room[id].title, strdup(title));
    strcpy(room[id].desc, strdup(desc));
    room[id].online = online;
    room[id].id = id;
    printf("ChatSystem >>> Pokalbiu kambarys sukurtas");
}
void client_create(char *name, int id_socket)
{
    int ids;
    room[0].online += 1;
    ids = room[0].online-1;
    members[ids].name = strdup(name);
    members[ids].socket = id_socket;
}
void client_destroy()
{
    room[0].online -= 1;
}
void client_change_name(int id_socket, char *name)
{
    int id;
    id = search_member(id_socket);
    members[id].name = strdup(name);
}
int search_member(int id_socket)
{
    int i = 0;
    for(i=0;i<room[0].online;i++)
    {
      if(members[i].socket == id_socket)
      {
          return i;
          break;
      }
    }
}
int strtoint_n(char* str, int n)
{
    int sign = 1;
    int place = 1;
    int ret = 0;

    int i;
    for (i = n-1; i >= 0; i--, place *= 10)
    {
        int c = str[i];
        switch (c)
        {
            case 45:
                if (i == 0) sign = -1;
                else return -1;
                break;
            default:
                if (c >= 48 && c <= 57) ret += (c - 48) * place;
                else return -1;
        }
    }

    return sign * ret;
}

int strtoint(char* str)
{
    char* temp = str;
    int n = 0;
    while (*temp != '\0')
    {
        n++;
        temp++;
    }
    return strtoint_n(str, n);
}
