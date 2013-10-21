

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
int ParseCommandInput ( char* UserInput, struct s_user *user )
{
    char Packet [2000] = {0}; // Buferis, skirtas saugoti PDU po atpazintos komandos.
    if ( (UserInput [0] == 'S' || UserInput [0] == 's') &&
            (UserInput [1] == 'E' || UserInput [1] == 'e') &&
            (UserInput [2] == 'T' || UserInput [2] == 't') &&
            (UserInput [3] == 'N' || UserInput [3] == 'n') &&
            (UserInput [4] == ' ') )
    {
        strcpy (Packet, UserInput+5);
        strcpy (Packet, "System >> ");
        strcat(Packet, user->name);
        client_change_name(user, UserInput+5);
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
    if ( (UserInput [0] == 'R' || UserInput [0] == 'r') &&
            (UserInput [1] == 'O' || UserInput [1] == 'o') &&
            (UserInput [2] == 'O' || UserInput [2] == 'o') &&
            (UserInput [3] == 'M' || UserInput [3] == 'm') &&
            (UserInput [4] == 'S' || UserInput [4] == 's') )
    {
        strcpy (Packet, "System >>");
        int n;
        for(n=0;n<5;n++)
        {
            strcat(Packet, room[n].title);
            strcat(Packet, " - ");
            strcat(Packet, room[n].desc);
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
        if(user->id_channel >= 0) {
            strcpy(Packet, user->name);
            strcat(Packet, ": ");
            strcat(Packet, UserInput+3);
        } else {
            strcpy(Packet, "System >> Turite prisijungti prie kanalo!\n");
        }
        MarshalPacket (Packet);
        strcpy (UserInput, Packet);
        return 3;
    }
    if ( (UserInput [0] == 'J' || UserInput [0] == 'j') &&
            (UserInput [1] == 'O' || UserInput [1] == 'o') &&
            (UserInput [2] == 'I' || UserInput [2] == 'i') &&
            (UserInput [3] == 'N' || UserInput [3] == 'n') &&
            (UserInput [4] == ' ') )
    {
        strcpy (Packet, UserInput+5);
        int room_id = strtoint(UserInput+5);
        strcpy (Packet, "System >> ");
        if(room_id < CHATROOMS && room_id >= 0)
        {
            if(room[room_id].online < CHATMEMBERS)
            {
                if(user->id_channel != room_id) {
                    user->id_channel = room_id;
                    room[room_id].people[room[room_id].online] = user->socket;
                    room[room_id].online += 1;
                    strcat(Packet, "Sekmingai prisijungta prie kanalo!");
                } else {
                    strcat(Packet, "Esate prisijunges prie sio kanalo!");
                }
            } else {
                strcat(Packet, "Kanale per daug zmoniu!");
            }
        } else
        {
            strcat(Packet, "Toks kanalas nerastas!");
        }
        MarshalPacket (Packet);
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
struct s_user *client_create(char *name, int id_socket)
{
    struct s_user *who = malloc(sizeof(struct s_user));
    assert(who != NULL);

    who->name = strdup(name);
    who->socket = id_socket;

    return who;
}
void client_destroy(struct s_user *who)
{
    assert(who != NULL);

    free(who->name);
    free(who);
}
void client_change_name(struct s_user *who, char *name)
{
    who->name = strdup(name);
}
void client_print(struct s_user *who)
{
    printf("Name: %s\n", who->name);
    printf("\tSocket: %d\n", who->socket);
    printf("\tRoom: %d\n", who->id_channel);
}
void create_rooms()
{
    create_room("Baras", "Test", 0, 0);
    create_room("Baras 2", "Bendras pokalbiu kamabarys", 5, 1);
    create_room("Baras 3", "Bendras pokalbiu kamabarys", 0, 2);
    create_room("Baras 4", "Bendras pokalbiu kamabarys", 0, 3);
    create_room("Baras 5", "Bendras pokalbiu kamabarys", 0, 4);
    printf("CHATSYSTEM >> Rooms created!\n");
}
void create_room(char *title, char *desc, int online, int id)
{
    strcpy(room[id].title, strdup(title));
    strcpy(room[id].desc, strdup(desc));
    room[id].online = online;
    room[id].id = id;
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
