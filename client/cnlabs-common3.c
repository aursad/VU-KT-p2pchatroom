#include	"cnlabs-common3.h"

// Suformuoti paketà, paruoðti duomenis persiuntimui tinklu
void MarshalPacket ( char* Packet )
{

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
int UnmarshalPacket ( char* Packet )
{
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
int SendPacket ( SOCKET* s, const char* Packet, int MaxBufferSize)
{
    int SentBytes = 0;             // Jau issiustu baitu skaicius.
    int BytesLeft = MaxBufferSize; // Kiek baitu dar liko issiusti.
    int nBytes;                    // Per viena karta issiunciamu baitu sk.

    //siusti duota paketa buferyje
    while( SentBytes < MaxBufferSize )
    {
        //bandoma issiusti dali duomenu
        nBytes = send ((*s), Packet + SentBytes, BytesLeft, 0);

        //Jei klaida nutraukia tikrinima ir pranesa vartotojui
        if ( nBytes == SOCKET_ERROR )
        {
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
int ReceivePacket ( SOCKET* s, char* Packet)
{
    char DataBuffer [2000] = {0};      // Duomenu buferis.
    int nBytes;                        // Baitu, gautu vienu kreipiniu i recv(), skaicius.

    //kreipiamasi norint gauti dali informacijos
    nBytes = recv ((*s), DataBuffer, sizeof (DataBuffer), 0);
    //Atlikti reikiamus veiksmus tolesniam darbui pagal nBytes reiksmæ. Jei nBytes == 0, tai reiðkia, kad vartotojas nutraukë ryðá su serveriu.
    if ( 0 == nBytes ) return 0;

    //Prieðingu atveju, praneðame, kad skaitant ávyko klaida.
    else if ( SOCKET_ERROR == nBytes ) return SOCKET_ERROR;

    //Prieðingu atveju, mes perskaitëme kaþkokià dalá informacijos. Jà reikia apiforminti taip, kaip to laukia HandleDataFromClient().
    else
    {
        memset (Packet, 0, nBytes);
        strcpy (Packet, DataBuffer);
    }
    //printf("Recv bytes: %d\n", nBytes);
    //grazina sekmes koda
    return 1;
}


