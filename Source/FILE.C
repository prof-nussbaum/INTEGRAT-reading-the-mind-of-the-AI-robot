#include "FUNCTIONS.H"

static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam);
#include <mem.h>
#include <memory.h>
#include <mmsystem.h>
#include <shellapi.h>
#define HALFMEG (unsigned long)(1024.0 * 1024.0 * 0.5)

extern char infilename[255];
extern void CMUFileOpen( HWND hWnd );
extern CONFIG_STRUCT config;
extern HINSTANCE hInst;
extern HWND      mainhwnd;                     // hWnd of main window

unsigned long load_up_file(HWND hWnd, MEMORY_FILE *mfile, int voice);
unsigned long load_up_config(HWND hWnd, MEMORY_FILE *mfile, int voice);
unsigned long save_file(HWND hWnd, MEMORY_FILE *mfile, int voice);
unsigned long save_config(HWND hWnd, MEMORY_FILE *mfile, int voice);

extern HANDLE    hdlg;                         // handle of dialog resource
static DLGPROC  lpDlgProc;                    // ptr to proc for dialog box
static char 	action;
static char savecon[255], savetra[255], savecod[255];

char *max_pos;

fileopen(HWND hWnd) {
	char *c1, *c2, *end;
	unsigned long size;
    char message[255];

	clearseg();//Remove segments from last file
	for (c1 = infilename, c2 = config.file.open.lpszFilename;
		*c2; c1++, c2++)
		*c1 = *c2;
    *c1 = 0x00;

	CMUFileOpen(hWnd);

	for (c1 = infilename, c2 = config.file.open.lpszFilename;
		*c1; c1++, c2++)
		*c2 = *c1;
	*c2 = 0x00;

	if (!(size = load_up_file(hWnd, &config.file.open, 1)))
		MessageBox(hWnd,"Open FAILED","Voice File Results",MB_OK);
	else {
		sprintf(message, "%g bytes loaded in RAM", (float)size);
		//MessageBox(hWnd,message,"Voice File Results",MB_OK);
		autosegment(hWnd);//Calculate Segments
	}
//	drawwav(hWnd);
}
filecon(HWND hWnd) {

	/* call the RAM archival command dialog box */
	cmrambox(hWnd);
	 
}
fileplay(HWND hWnd) {
		DWORD res;
    int button = 0;
		static char Buffer[200], Buffer1[100];

	button = FindExecutable(config.file.open.lpszFilename,
		"D:\\WIN", Buffer);
	while (button <= 32)
    {
		button = MessageBox (hWnd, "Play File?", "Play File", MB_YESNO);
		if (button == IDYES) {
			CMUFileOpen(hWnd);
			button = FindExecutable(infilename, "D:\\WIN", Buffer);
		}
		else
			return(0);
	}//end trying to read files
	button = ShellExecute(hWnd, "open",Buffer,infilename,"D:\\WIN",SW_SHOWNORMAL);
    return FALSE;
/* Don't Use the MMI String method below because it Always uses Stereo
		sprintf(Buffer, "open %s type waveaudio alias sound", infilename);
		res = mciSendString ((LPSTR)Buffer, Buffer1, 80, NULL);
		while (res) {
			mciGetErrorString(res, Buffer, 100);
	  sprintf(Buffer1, "Play %s", infilename);
			MessageBox (hWnd, Buffer, Buffer1, MB_OK);
			button = MessageBox (hWnd, "Play File?", "Play File", MB_YESNO);
			if (button == IDYES) {
				CMUFileOpen(hWnd);
				sprintf(Buffer, "open %s type waveaudio alias sound", infilename);
				res = mciSendString ((LPSTR)Buffer, Buffer1, 80, NULL);
			}
			else
				return(0);
		}//end trying to read files
		res = mciSendString ((LPSTR)"play sound", Buffer1, 80, NULL);
		if (res) {
			mciGetErrorString(res, Buffer, 100);
			MessageBox (hWnd, Buffer, "Record to File", MB_OK);
    }
		else
			MessageBox (hWnd, "Press Return (or OK) to Stop", "Play Say When", MB_OK);
		res = mciSendString ((LPSTR)"stop sound", Buffer1, 80, NULL);
		res = mciSendString ((LPSTR)"close sound", Buffer1, 80, NULL);
*/
}
filerec(HWND hWnd) {
		DWORD res;
    int button = 0;
		static char Buffer[200], Buffer1[100];
		char *c1, *c2;
	unsigned long size;

/*	while (button <= 32)
    {
		MessageBox (hWnd, Buffer, Buffer1, MB_OK);
		button = MessageBox (hWnd, "Record File?", "Play File", MB_YESNO);
		if (button == IDYES) {
			CMUFileOpen(hWnd);
			sprintf(Buffer, "open %s type waveaudio alias sound", infilename);
			button = FindExecutable(infilename, "D:\\WIN", Buffer);
		}
		else
			return(0);
	}//end trying to read files
*/
	button = ShellExecute(hWnd, "open","D:\\WIN\\SOUNDREC.EXE","",
		"C:\\TEST",SW_SHOWNORMAL);
/*
	if (button > 32)
	{
		if (!(size = load_up_file(hWnd, &config.file.open)))
			MessageBox(hWnd,"Memory Load FAILED","Record Post-Processing",MB_OK);
		else {
				sprintf(Buffer, "%d bytes loaded in RAM", size);
				MessageBox(hWnd,Buffer,"Voice File Results",MB_OK);
		}
	}
	return FALSE;
*/
/* Don't use MMI String Method wit PC Speaker
			button = MessageBox (hWnd, "Create New File?", "Record to File", MB_YESNO);
			if (button == IDYES) {
      	infilename[0] = 0;
				sprintf(Buffer, "open %s type waveaudio alias sound buffer 6", "new");
				res = mciSendString ((LPSTR)Buffer, Buffer1, 80, NULL);
        if (res) { 
					mciGetErrorString(res, Buffer, 100);
					MessageBox (hWnd, Buffer, "Record to File", MB_OK);
					return(0);
				}
      }
     	else {
				CMUFileOpen(hWnd);

				sprintf(Buffer, "open %s type waveaudio alias sound buffer 6", infilename);
				res = mciSendString ((LPSTR)Buffer, Buffer1, 80, NULL);
				if (res) {
					mciGetErrorString(res, Buffer, 100);
					MessageBox (hWnd, Buffer, "Record to File", MB_OK);
					return(0);
        }
			}
		button = MessageBox (hWnd, "Record Start?", "Record to File", MB_YESNO);
		if (button == IDYES) {
			res = mciSendString ((LPSTR)"record sound", Buffer, 80, NULL);
			if (res) {
					mciGetErrorString(res, Buffer, 100);
					MessageBox (hWnd, Buffer, "Record to File", MB_OK);
					return(0);
			}
    }
		else return(0);
		MessageBox (hWnd, "Press Return (or OK) to Stop Recording", "Record Say When", MB_OK);
		res = mciSendString ((LPSTR)"stop sound", NULL, 0, NULL);
		if (!infilename[0]) 
    	CMUFileOpen(hWnd);
		sprintf(Buffer, "save sound %s wait", infilename);
    res = mciSendString ((LPSTR)Buffer, NULL, 0, NULL);
		if (!res) {
					MessageBox (hWnd, "File Saved", "Record SUCCESS", MB_OK);
					for (c1 = infilename, c2 = config.file.open.lpszFilename;
						*c1; c1++, c2++)
						*c2 = *c1;
					*c2 = 0x00;

					if (!(size = load_up_file(hWnd, &config.file.open)))
						MessageBox(hWnd,"Memory Load FAILED","Record Post-Processing",MB_OK);
					else {
							sprintf(Buffer, "%d bytes loaded in RAM", size);
							MessageBox(hWnd,Buffer,"Voice File Results",MB_OK);
					}
		}
		else {
					mciGetErrorString(res, Buffer, 100);
					MessageBox (hWnd, Buffer, "Record to File", MB_OK);
		}
		res = mciSendString ((LPSTR)"close sound", NULL, 0, NULL);
*/

}

//*******************************************************************
// DlgBoxProc - handle dialog messages
//
// parameters:
//             hDlg          - The window handle for this message
//             message       - The message number
//             wParam        - The WPARAM parameter for this message 
//             lParam        - The LPARAM parameter for this message
//
//*******************************************************************
static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam)
{
	char *c1, *c2, *end;

	switch (message) {
	case WM_CREATE:
	case WM_PAINT:
		validate_names();
		SetDlgItemText(hDlg, RAM_CON, savecon);
		SetDlgItemText(hDlg, RAM_TRA, savetra);
		SetDlgItemText(hDlg, RAM_COD, savecod);
		SetFocus(hDlg);
		return FALSE;

    case WM_SYSCOMMAND:
            // Pass WM_SYSCOMMAND messages on to main window so both
            // main window and dialog box get iconized, minimized etc.
			// in parallel.
		SendMessage(mainhwnd, message, wParam, lParam);
		break;

	case WM_COMMAND:
		switch( wParam) {
			case RAM_LOAD:
				action = 1;//load
				GetDlgItemText(hDlg, RAM_CON, savecon, 200);
				GetDlgItemText(hDlg, RAM_TRA, savetra, 200);
				GetDlgItemText(hDlg, RAM_COD, savecod, 200);
				EndDialog(hDlg, TRUE);
				break;
			case IDOK:
				action = 0;//do nothing
				EndDialog(hDlg, TRUE);
				break;
            case RAM_SAVE:
				action = 2;//save
				GetDlgItemText(hDlg, RAM_CON, savecon, 200);
				GetDlgItemText(hDlg, RAM_TRA, savetra, 200);
				GetDlgItemText(hDlg, RAM_COD, savecod, 200);
				EndDialog(hDlg, TRUE);
				break;
			case RAM_REFRESH:
				GetDlgItemText(hDlg, RAM_CON, savecon, 200);
				SendMessage(hDlg, WM_PAINT, 0, 0);
				break;
			case RAM_BROWSE:
				for (c1 = infilename, c2 = savecon;
					*c2; c1++, c2++)
					*c1 = *c2;
			    *c1 = 0x00;
				CMUFileOpen(hDlg);
				for (c1 = infilename, c2 = savecon;
					*c1; c1++, c2++)
					*c2 = *c1;
				*c2 = 0x00;
				infilename[0] = 0x00;
				SendMessage(hDlg, WM_PAINT, 0, 0);
				return FALSE;
			default:
        		return FALSE;
		}//End of switch on command
		break;

  		default:
    		return FALSE;
 }  // switch

 return(TRUE);
}  // end of DlgBoxProc()
//*************************************************************************
// cmgetnum -- invoke the "getnum" dialog.  Its return code is ignored, since
// the getnum dialog doesn't return anything to the program.
//*************************************************************************
cmrambox(HWND hWnd)
{

	char *c1, *c2, *end;
    char message[255];
	unsigned long size;
    int x;

	/* save current filenames */
	for (c1 = savecon, c2 = config.file.con.lpszFilename;
		*c2; c1++, c2++)
		*c1 = *c2;
    *c1 = 0x00;
	for (c1 = savetra, c2 = config.file.tra.lpszFilename;
		*c2; c1++, c2++)
		*c1 = *c2;
    *c1 = 0x00;
	for (c1 = savecod, c2 = config.file.cod.lpszFilename;
		*c2; c1++, c2++)
		*c1 = *c2;
	*c1 = 0x00;

	/* Assume failed*/
	action = -1;//failed

    while (action == -1) {
		DialogBox(hInst, "RAM_BOX", mainhwnd,
			(DLGPROC) DlgBoxProc);

		/* replace as necessary */
		if (action) {//load or save
			validate_names();
			for (c1 = savecon, c2 = config.file.con.lpszFilename;
				*c1; c1++, c2++)
				*c2 = *c1;
			*c2 = 0x00;
			for (c1 = savetra, c2 = config.file.tra.lpszFilename;
				*c1; c1++, c2++)
				*c2 = *c1;
		    *c2 = 0x00;
			for (c1 = savecod, c2 = config.file.cod.lpszFilename;
				*c1; c1++, c2++)
				*c2 = *c1;
			*c2 = 0x00;
		}
		if (action == 1) {//load
			if (!(size = load_up_config(hWnd, &config.file.con, 0))) {
				MessageBox(hWnd,".CON Load FAILED","Configuration File Results",MB_OK);
				action = -1;
            }
			else {
				sprintf(message, "%g bytes loaded in RAM", (float)size);
				MessageBox(hWnd,message,"Configuration File Results",MB_OK);
			}
			/* replace configuration filenames with user selected */
			/* in case user has changed the file's names */
			for (c1 = savecon, c2 = config.file.con.lpszFilename;
				*c1; c1++, c2++)
				*c2 = *c1;
			*c2 = 0x00;
			for (c1 = savetra, c2 = config.file.tra.lpszFilename;
				*c1; c1++, c2++)
				*c2 = *c1;
		    *c2 = 0x00;
			for (c1 = savecod, c2 = config.file.cod.lpszFilename;
				*c1; c1++, c2++)
				*c2 = *c1;
			*c2 = 0x00;
			if (!(size = load_up_file(hWnd, &config.file.tra, 0))) {
				MessageBox(hWnd,".TRA Load FAILED","Training File Results",MB_OK);
				action = -1;
				config.id.count = 0;
				for (x = 0; x < 128; x++) {
					config.id.ids[x].examples = 0;
				}
            }
			else {
				sprintf(message, "%g bytes loaded in RAM", (float)size);
				MessageBox(hWnd,message,"Training File Results",MB_OK);
			}
			if (!(size = load_up_file(hWnd, &config.file.cod, 0))) {
				MessageBox(hWnd,".COD Load FAILED","Coding File Results",MB_OK);
				action = -1;
				config.code.iter = 0;
                config.code.on = 0;
            }
			else {
				sprintf(message, "%g bytes loaded in RAM", (float)size);
				MessageBox(hWnd,message,"Coding File Results",MB_OK);
			}
		}
		if (action == 2) {//save
			if (!(size = save_config(hWnd, &config.file.con, 0))) {
				MessageBox(hWnd,".CON Save FAILED","Configuration File Results",MB_OK);
				action = -1;
            }
			else {
				sprintf(message, "%g bytes saved to disk", (float)size);
				MessageBox(hWnd,message,"Configuration File Results",MB_OK);
			}
			if ((!(size = save_file(hWnd, &config.file.tra, 0))) &&
	        		(config.file.tra.size)) {
					MessageBox(hWnd,".TRA Save FAILED","Training File Results",MB_OK);
				action = -1;
            }
			else {
				sprintf(message, "%g bytes saved to disk", (float)size);
				MessageBox(hWnd,message,"Training File Results",MB_OK);
			}
			if ((!(size = save_file(hWnd, &config.file.cod, 0))) &&
					(config.file.cod.size)) {
				MessageBox(hWnd,".COD Save FAILED","Coding File Results",MB_OK);
				action = -1;
            }
			else {
				sprintf(message, "%g bytes Saved to Disk", (float)size);
				MessageBox(hWnd,message,"Coding File Results",MB_OK);
			}

        }
	}
}  // end of user I/O
validate_names(){
	/* all file names must be identical except
		for correct extension */
	char *ptra, *pcod, *pcon, *pext;
	char *traext = ".TRA";
	char *codext = ".COD";
	char *conext = ".CON";

	pcon = savecon;
	ptra = savetra;
	pcod = savecod;

	while ((*pcon) &&
		   !((*(pcon + 1) == '.') &&
			(*pcon == '\\')) &&
			(*(pcon + 1) != '.')) {
		*ptra = *pcon;
		*pcod = *pcon;
		pcon++; ptra++; pcod++;
	}
	if (*pcon) {
		*ptra = *pcon;
		*pcod = *pcon;
		pcon++; ptra++; pcod++;
	}
	if (pcon > &savecon[200]) {
    	savecon[0] = 0x00;
		savetra[0] = 0x00;
		savecod[0] = 0x00;
		return FALSE;
    }
	pext = conext;
	while (*pext) *(pcon++) = *(pext++);
	pext = traext;
	while (*pext) *(ptra++) = *(pext++);
	pext = codext;
	while (*pext) *(pcod++) = *(pext++);

    *pcod = *pcon = *ptra = 0x00;
    return TRUE;
}
unsigned long load_up_file(HWND hWnd, MEMORY_FILE *mfile, int voice) {
	unsigned long size;
	OFSTRUCT ofFile[1];
	HFILE result;
	char *cptr, *end;
    float max_amplitude, scale;
	HCURSOR hcurSave;

	ofFile[0].cBytes = 0;
	ofFile[0].fFixedDisk = 0;
	ofFile[0].nErrCode = 0;
//	ofFile[0].reserved[0] = 0;
	ofFile[0].Reserved1 = 0;
	ofFile[0].Reserved2 = 0;
	ofFile[0].szPathName[0] = 0;
	mfile->fnOpenMode = OF_CANCEL | OF_PROMPT | OF_READ;
	mfile->hpFile = OpenFile(mfile->lpszFilename, (OFSTRUCT FAR*)ofFile, mfile->fnOpenMode);
	if (mfile->hpFile == HFILE_ERROR) {
			if (mfile->data) free(mfile->data); // farfree(mfile->data);
			mfile->data = 0;
			mfile->size = 0;
			mfile->cur_pos = 0;
			mfile->lpszFilename[0] = 0;
			MessageBox(hWnd,"Can't Open File!","Load File Results",MB_OK);
			result = _lclose(mfile->hpFile);
			return(0);
	}
//	if ((!mfile->data) && (!(mfile->data = farcalloc(1, HALFMEG)))) {
	if ((!mfile->data) && (!(mfile->data = (char *) calloc(1, HALFMEG)))) {
			MessageBox(hWnd,"Can't Alloc 1/2 MEG!","Voice File Results",MB_OK);
			result = _lclose(mfile->hpFile);
			return(0);
    }
	for (mfile->size = 0, mfile->cur_pos = mfile->data;
			mfile->size < HALFMEG;) {
		size = _lread(mfile->hpFile, mfile->cur_pos, (unsigned int)(1024.0 * .25));
		mfile->size += size;
		mfile->cur_pos += size;
        if ((size < (unsigned int)(1024.0 * .25)) || (!size))
            break;
	}
    if (size == HFILE_ERROR)
		MessageBox(hWnd,"Read Unterminated!","Load File Results",MB_OK);
//	mfile->cur_pos = mfile->data;
    if (voice) {
		//Let the user know this is a time consuming process
		hcurSave = SetCursor(LoadCursor(hInst,"CUR_NORM" ));
		end = mfile->data + mfile->size - 1;
	    max_amplitude = 0;
		for(cptr = mfile->data + 100; cptr < end; cptr++) {
			*cptr = ((unsigned char)*cptr) - 127.0;
			if (*cptr > max_amplitude) {
				max_amplitude = *cptr;
				max_pos = cptr;
            }
			else if (-*cptr > max_amplitude){
				max_amplitude = -*cptr;
				max_pos = cptr;
            }
		}
		if (max_amplitude) {
	    	scale = 127 / max_amplitude;
			for(cptr = mfile->data + 64; cptr < end; cptr++) {
				*cptr *= scale;
			}
		}
		result = _lclose(mfile->hpFile);
		SetCursor(hcurSave);
	}
	result = _lclose(mfile->hpFile);
	return(mfile->size);
}
unsigned long load_up_config(HWND hWnd, MEMORY_FILE *mfile, int voice) {
	unsigned long size;
	OFSTRUCT ofFile[1];
	HFILE result;
	char *cptr, *end;
    float max_amplitude, scale;
	HCURSOR hcurSave;
    SEGMENT *segptr, *segend;

	mfile->data = (char *)&config;
	mfile->size = sizeof(CONFIG_STRUCT);
	mfile->cur_pos = (char *)&config;

	ofFile[0].cBytes = 0;
	ofFile[0].fFixedDisk = 0;
	ofFile[0].nErrCode = 0;
//	ofFile[0].reserved[0] = 0;
	ofFile[0].Reserved1 = 0;
	ofFile[0].Reserved2 = 0;
	ofFile[0].szPathName[0] = 0;
	mfile->fnOpenMode = OF_CANCEL | OF_PROMPT | OF_READ;
	mfile->hpFile = OpenFile(mfile->lpszFilename, (OFSTRUCT FAR*)ofFile, mfile->fnOpenMode);
	if (mfile->hpFile == HFILE_ERROR) {
			mfile->lpszFilename[0] = 0;
			MessageBox(hWnd,"Can't Open File!","Load File Results",MB_OK);
			result = _lclose(mfile->hpFile);
			return(0);
	}
	/* deallocate anything lying around */
	if (config.file.tra.data)
//		farfree(config.file.tra.data);
		free(config.file.tra.data);
	if (config.file.cod.data)
//		farfree(config.file.cod.data);
		free(config.file.cod.data);
	size = _lread(mfile->hpFile, mfile->cur_pos, sizeof(CONFIG_STRUCT));
	/* make sure configuration doesn.t retrieve voice data */
	config.file.open.data = 0;
	config.file.open.cur_pos = 0;
	config.file.open.size = 0;
	config.file.tra.data = 0;
	config.file.tra.cur_pos = 0;
	config.file.tra.size = 0;
	config.file.cod.data = 0;
	config.file.cod.cur_pos = 0;
	config.file.cod.size = 0;
	segend = &config.seg.segments[127];
	for (segptr = config.seg.segments; segptr <= segend; segptr++) {
		segptr->start = segptr->end = 0;
		segptr->tran_start = 0;
		segptr->class1 = segptr->class2 = 0;
		segptr->num_trans = 0;
	}
	if ((size < sizeof(CONFIG_STRUCT)) || (!size) ||(size == HFILE_ERROR))
		MessageBox(hWnd,"Read Unterminated!","Load File Results",MB_OK);
	mfile->cur_pos = mfile->data;
	result = _lclose(mfile->hpFile);
	return(mfile->size);
}
unsigned long save_config(HWND hWnd, MEMORY_FILE *mfile, int voice) {
	unsigned long size;
	OFSTRUCT ofFile[1];
	HFILE result;
	char *cptr, *end;
    float max_amplitude, scale;
	HCURSOR hcurSave;

	mfile->data = (char *)&config;
	mfile->size = sizeof(CONFIG_STRUCT);
	mfile->cur_pos = (char *)&config;

	ofFile[0].cBytes = 0;
	ofFile[0].fFixedDisk = 0;
	ofFile[0].nErrCode = 0;
//	ofFile[0].reserved[0] = 0;
	ofFile[0].Reserved1 = 0;
	ofFile[0].Reserved2 = 0;
	ofFile[0].szPathName[0] = 0;
	mfile->fnOpenMode = OF_CANCEL | OF_PROMPT | OF_CREATE | OF_WRITE;
	mfile->hpFile = OpenFile(mfile->lpszFilename, (OFSTRUCT FAR*)ofFile, mfile->fnOpenMode);
	if (mfile->hpFile == HFILE_ERROR) {
			mfile->lpszFilename[0] = 0;
			MessageBox(hWnd,"Can't Open File!","Save File Results",MB_OK);
			result = _lclose(mfile->hpFile);
			return(0);
	}
	size = _lwrite(mfile->hpFile, mfile->cur_pos, sizeof(CONFIG_STRUCT));
	mfile->size = size;
	if ((size < sizeof(CONFIG_STRUCT)) || (!size) || (size == HFILE_ERROR))
		MessageBox(hWnd,"Read Unterminated!","Save File Results",MB_OK);
	result = _lclose(mfile->hpFile);
	return(mfile->size);
}
unsigned long save_file(HWND hWnd, MEMORY_FILE *mfile, int voice) {
	unsigned long size, savesize;
	OFSTRUCT ofFile[1];
	HFILE result;
	char *cptr, *end;
    float max_amplitude, scale;
	HCURSOR hcurSave;

//	if ((!mfile->data) && (!(mfile->data = farcalloc(1, HALFMEG)))) {
	if ((!mfile->data) && (!(mfile->data = (char *) calloc(1, HALFMEG)))) {
			MessageBox(hWnd,"Can't Alloc 1/2 MEG!","Voice File Results",MB_OK);
			return(0);
    }
	ofFile[0].cBytes = 0;
	ofFile[0].fFixedDisk = 0;
	ofFile[0].nErrCode = 0;
//	ofFile[0].reserved[0] = 0;
	ofFile[0].Reserved1 = 0;
	ofFile[0].Reserved2 = 0;
	ofFile[0].szPathName[0] = 0;
	mfile->fnOpenMode = OF_CANCEL | OF_PROMPT | OF_CREATE | OF_WRITE;
	mfile->hpFile = OpenFile(mfile->lpszFilename, (OFSTRUCT FAR*)ofFile, mfile->fnOpenMode);
	if (mfile->hpFile == HFILE_ERROR) {
			MessageBox(hWnd,"Can't Open File!","Save File Results",MB_OK);
			result = _lclose(mfile->hpFile);
			return(0);
	}
	savesize = mfile->size;
	for (mfile->cur_pos = mfile->data;
		 savesize > 0;) {
		if (savesize >= (unsigned int)(1024.0 * .25))
			size = (unsigned int)(1024.0 * .25);
        else size = savesize;
		size = _lwrite(mfile->hpFile, mfile->cur_pos, size);
		savesize -= size;
		mfile->cur_pos += size;
        if ((size < (unsigned int)(1024.0 * .25)) || (!size))
            break;
	}
    if (size == HFILE_ERROR)
		MessageBox(hWnd,"Save Unterminated!","Save File Results",MB_OK);
	mfile->cur_pos = mfile->data + mfile->size;
	result = _lclose(mfile->hpFile);
	return(mfile->size);
}
