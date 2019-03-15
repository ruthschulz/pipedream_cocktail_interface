#include <windows.h>
#include <string.h>
#include <commctrl.h>

#include "serial.h"

const int	inQueue			    = 4096;
const int	outQueue			= 4096;

/*
 * InitComms -
 *
 * This function initalises the serial port to the transfer data with the
 * settings given by mode. Port is a string set as "COMx" where x is the
 * specified port beginning with 0. mode is a string of the form:
 * "baud=4800 parity=E data=8 stop=1"
 *
 * RETURN VALUE: TRUE if successful, FALSE if not
 */
int serialpcInitComms(Comms *cp, char *port, char *mode)
{
	int	err;
	DCB	dcb;

	/* Open the com port "port" in overlapped mode for writing and reading */
	if ((cp->hComms = CreateFile(port, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)) == INVALID_HANDLE_VALUE)
		return (FALSE);

	/* Set the Comm port mode */
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);

	if ((err = BuildCommDCB(mode, &dcb)) == 0)
		return (FALSE);

	if ((err = SetCommState(cp->hComms, &dcb)) == 0)
		return (FALSE);

	/* Setup the queues */
	if (!SetupComm(cp->hComms, inQueue, outQueue))
		return (FALSE);

	/* Send the DTR (data-terminal-ready) signal. */
	EscapeCommFunction(cp->hComms, SETDTR);

	/* Fill the overlapped structure */
	cp->ol.Offset = 0;
	cp->ol.OffsetHigh = 0;

	/* Create the overlapped event */
	if ((cp->ol.hEvent = CreateEvent(NULL, FALSE, FALSE, "WaitPORT")) == NULL)
		return (FALSE);

	/* Set the first time flag */
	cp->first = TRUE;

	return (TRUE);
}


/*
 * CloseComms -
 *
 * close the communications with the given handle
 */
void serialpcCloseComms(Comms *cp)
{
	FlushFileBuffers(cp->hComms);
	CloseHandle(cp->ol.hEvent);	
	CloseHandle(cp->hComms);
}


/*
 * SendData -
 *
 * This function sends the data to teh serial device driver. It does not
 * wait until the device has sent the data before returning
 *
 * M_FIX (DB): I think that this function should be looked at and possibly rewritten.
 *
 * RETURN VALUE: TRUE if successful, FALSE if failure
 */
DWORD SendData(Comms *cp, char *buff, int count)
{
	LPDWORD	written = 0;
	int error;
	COMSTAT comStat;
    DWORD   dwErrors;

	/* If not the first time then wait for last transfer to complete */
	if (!cp->first) {

		FlushFileBuffers(cp->hComms);

//		int data_in_buffer = TRUE;

//		while (data_in_buffer) {
			if (!ClearCommError(cp->hComms, &dwErrors, &comStat)) {
			    // Report error in ClearCommError.
				return (COMMS_ERROR_IN_CLEAR_COMMS);
			}
		// If not data in buffer
		  if (comStat.cbInQue)
			  PurgeComm(cp->hComms, PURGE_TXABORT);

//				data_in_buffer = FALSE;
			//		return (COMMS_ERROR_DATA_IN_BUFFER);
//		}
	}
	cp->first = FALSE;

	WriteFile(cp->hComms, buff, count, written, &(cp->ol));

	error = GetLastError();
	if (error != ERROR_IO_PENDING)
		return (COMMS_ERROR_WRITE_FILE);

	return (COMMS_ARE_GOOD);

}

