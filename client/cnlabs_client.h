#ifndef CNLABS_CLIENT_H
#define	CNLABS_CLIENT_H

#include "../cnlabs-common3.h"

SOCKET InitializeClient ( void );
int ParseCommandInput ( char* UserInput );

#endif
