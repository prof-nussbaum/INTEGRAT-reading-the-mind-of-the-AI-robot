#include "FUNCTIONS.H"
#include <mmsystem.h>
extern CONFIG_STRUCT config;
extern HINSTANCE hInst;

static int prate = 22;

playseg(HWND hWnd, SEGMENT *segptr) {
	WAVEHDR header;
	HWAVEOUT handle;
	unsigned int device_id;
	WAVEFORMAT format;
	unsigned int error;
	WAVEOUTCAPS caps;
	HGLOBAL hglob;
	void far *lpglob;
	char *hstart;
	char *hend;
    char *hptr;
	unsigned char far *fptr;
	DWORD count;
    int repeats;
	HCURSOR hcurSave;

	if (!segptr->start)
		return FALSE;
	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst,"CUR_PLAY" ));
	hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, (DWORD)65535);
	lpglob = GlobalLock(hglob);
	if (!lpglob) return FALSE;
	hstart = segptr->start;
	hend = segptr->end;
	count = hend - hstart;
    hptr = hstart;
    if (count > 65535 - 44) count = 65535 - 44;
	fptr = (char far *)lpglob + 44;
	for (; count; count--){
		*(fptr++) = *(hptr++) + 127;
	}
	fake_header(lpglob, (DWORD)(hend - hstart));
	/* repeat until a quarter second is heard */
    if (count = (hend - hstart)) {
		repeats = 5513.0 / count;
	    while (repeats >= 0) {
			sndPlaySound(lpglob, SND_SYNC | SND_MEMORY);
			repeats--;
		}
	} 
	GlobalUnlock(hglob);
    GlobalFree(hglob);
	SetCursor(hcurSave);
}
playrate(HWND hWnd){
	int button;

	if (prate == 22) {
		button = MessageBox (hWnd, "Change to 11.025Kbps?", "Rate is 22.050Kbps", MB_YESNO);
		if (button == IDYES) {
			prate = 11;
		}
	}
	else {
    	prate = 11;
		button = MessageBox (hWnd, "Change to 22.050Kbps?", "Rate is 11.025Kbps", MB_YESNO);
		if (button == IDYES) {
			prate = 22;
		}
	}
}
fake_header(char far *lptr, DWORD count){

	*lptr++ = 'R';
	*lptr++ = 'I';
	*lptr++ = 'F';
	*lptr++ = 'F';

	*lptr++ = (count + 36) & 0x00ff;
	*lptr++ = ((count + 36) & 0xff00) >> 8;
	*lptr++ = 0;
	*lptr++ = 0;

	*lptr++ = 'W';
	*lptr++ = 'A';
	*lptr++ = 'V';
	*lptr++ = 'E';

	*lptr++ = 'f';
	*lptr++ = 'm';
	*lptr++ = 't';
	*lptr++ = ' ';

	*lptr++ = 0x10;
	*lptr++ = 0x00;
	*lptr++ = 0x00;
	*lptr++ = 0x00;

	*lptr++ = 0x01;
	*lptr++ = 0x00;
	*lptr++ = 0x01;
	*lptr++ = 0x00;

	if (prate == 22) {
		*lptr++ = 0x22;
		*lptr++ = 0x56;
	}
	else {     
		*lptr++ = 0x11;
		*lptr++ = 0x2b;
    }
	*lptr++ = 0x00;
	*lptr++ = 0x00;

	if (prate == 22) {
		*lptr++ = 0x22;
		*lptr++ = 0x56;
	}
	else {     
		*lptr++ = 0x11;
		*lptr++ = 0x2b;
    }
	*lptr++ = 0x00;
	*lptr++ = 0x00;

	*lptr++ = 0x01;
	*lptr++ = 0x00;
	*lptr++ = 0x08;
	*lptr++ = 0x00;

	*lptr++ = 'd';
	*lptr++ = 'a';
	*lptr++ = 't';
	*lptr++ = 'a';

	*lptr++ = (count - 1) & 0x00ff;
	*lptr++ = ((count - 1) & 0xff00) >> 8;
	*lptr++ = 0;
	*lptr++ = 0;
}
