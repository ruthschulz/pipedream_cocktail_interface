#ifndef _SERIALPC_H
#define _SERIALPC_H

#include <windows.h>
//#include "ctypes.h"

/* Comms structure for communicating with the comms port */
typedef struct {
	HANDLE		hComms;
	OVERLAPPED	ol;
	BOOL		first;
} Comms;

/* Parity types */
#define COMMS_PARITY_NONE	0
#define COMMS_PARITY_ODD	1
#define COMMS_PARITY_EVEN	2
#define COMMS_PARITY_MARK	3
#define COMMS_PARITY_SPACE	4

#define COMMS_ARE_GOOD				0
#define COMMS_ERROR_WRITE_FILE		1
#define COMMS_ERROR_DATA_IN_BUFFER	2
#define COMMS_ERROR_IN_CLEAR_COMMS	3

/*
 * init_comms -
 *
 * This function initalises the serial port to the transfer data with the
 * settings given by mode. Port is a string set as "COMx" where x is the
 * specified port beginning with 0. mode is a string of the form:
 * "baud=4800 parity=E data=8 stop=1"
 *
 * RETURN VALUE: a valid handle to the port, NULL if an errror occurred.
 */
int serialpcInitComms(Comms *cp, char *port, char *mode);


/*
 * CloseComms -
 *
 * close the communications with the given handle
 */
void serialpcCloseComms(Comms *cp);

/*
 * send_data -
 *
 * This function sends the data to teh serial device driver. It does not
 * wait until the device has sent the data before returning
 *
 * RETURN VALUE: 0 if successful, error code if failure
 */
DWORD SendData(Comms *cp, char *buff, int count);

#endif /* _SERIALPORT_H */
