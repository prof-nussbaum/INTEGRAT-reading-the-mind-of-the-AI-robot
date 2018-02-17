#include "FUNCTIONS.H"
#include <stdlib.h>
#include <math.h>

extern HINSTANCE hInst;                        // hInstance of application
extern HWND      mainhwnd;                     // hWnd of main window

extern HANDLE    hdlg;                         // handle of dialog resource
extern CONFIG_STRUCT config;
static BOOL CALLBACK  DlgBoxProc(HWND hDlg, unsigned int message,
							 WPARAM wParam, LPARAM lParam);


static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam);
static DLGPROC  lpDlgProc;                    // ptr to proc for dialog box
static BOOL 	truth;
static int      id_disp = 1;
static char 	string[255];
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


    switch (message) {
	case WM_CREATE:
		id_disp = 1;
	case WM_PAINT:
		SetDlgItemText(hDlg, CB_CONFILE, config.file.con.lpszFilename);
		SetDlgItemText(hDlg, CB_TRAFILE, config.file.tra.lpszFilename);
		SetDlgItemText(hDlg, CB_CODFILE, config.file.cod.lpszFilename);
		SetDlgItemInt(hDlg, CB_THRESH, config.seg.lev, TRUE);
		SetDlgItemInt(hDlg, CB_DUR, config.seg.dur, TRUE);
		SetDlgItemInt(hDlg, CB_MIN, config.seg.min, TRUE);
		SetDlgItemInt(hDlg, CB_MAX, config.seg.max, TRUE);
		SetDlgItemInt(hDlg, CB_SIZE, config.tran.size, TRUE);
		SetDlgItemInt(hDlg, CB_INTERVAL, config.tran.interval, TRUE);
		SetDlgItemInt(hDlg, CB_DIM, config.tran.dim, TRUE);
		if (!config.tran.on)
			sprintf(string,"No Transform in Use");
		else
			if (config.tran.fft) 
				if (config.tran.rms)
					if (config.tran.nopitch)
						if (config.tran.hamm)
							sprintf(string,"FFT, Megnitude, No Pitch, Hamming");
						else
							sprintf(string,"FFT, Megnitude, No Pitch, Rectangular");
					else
						if (config.tran.hamm)
							sprintf(string,"FFT, Megnitude, Hamming");
						else
							sprintf(string,"FFT, Megnitude, Rectangular");
				else
					if (config.tran.nopitch)
						if (config.tran.hamm)
							sprintf(string,"FFT, COS, No Pitch, Hamming");
						else
							sprintf(string,"FFT, COS, No Pitch, Rectangular");
					else
						if (config.tran.hamm)
							sprintf(string,"FFT, COS, Hamming");
						else
							sprintf(string,"FFT, COS, Rectangular");
            else
				if (config.tran.hamm)
					sprintf(string,"LPC, Hamming");
				else
					sprintf(string,"LPC, Rectangular");
		SetDlgItemText(hDlg, CB_TDESC, string);
		if (config.code.on)
			if (config.code.ucl)
				sprintf(string, "UCL Coding");
			else if (config.code.autoassc)
				sprintf(string, "Auto-Associative");
			else if (config.code.scl)
				sprintf(string, "SCL Coding");
			else 
				sprintf(string, "Backprop Coding");
		else
			sprintf(string, "No Coding In Use");
		SetDlgItemText(hDlg, CB_CSUMM, string);
		sprintf(string, "%li", config.code.iter);
		SetDlgItemText(hDlg, CB_TITER, string);
		sprintf(string, "%li", config.file.open.size);
		SetDlgItemText(hDlg, CB_VSIZE, string);
		sprintf(string, "%li", config.id.count);
		SetDlgItemText(hDlg, CB_NEXAM, string);
		SetDlgItemInt(hDlg, CB_NID, config.id.num, TRUE);
        /* display seven ID's*/
		SetDlgItemInt(hDlg, CB_ID1, id_disp, TRUE);
		SetDlgItemInt(hDlg, CB_ID2, id_disp + 1, TRUE);
		SetDlgItemInt(hDlg, CB_ID3, id_disp + 2, TRUE);
		SetDlgItemInt(hDlg, CB_ID4, id_disp + 3, TRUE);
		SetDlgItemInt(hDlg, CB_ID5, id_disp + 4, TRUE);
		SetDlgItemInt(hDlg, CB_ID6, id_disp + 5, TRUE);
		SetDlgItemInt(hDlg, CB_ID7, id_disp + 6, TRUE);
		SetDlgItemText(hDlg, CB_NAME1, config.id.ids[id_disp + 0].name);
		SetDlgItemText(hDlg, CB_NAME2, config.id.ids[id_disp + 1].name);
		SetDlgItemText(hDlg, CB_NAME3, config.id.ids[id_disp + 2].name);
		SetDlgItemText(hDlg, CB_NAME4, config.id.ids[id_disp + 3].name);
		SetDlgItemText(hDlg, CB_NAME5, config.id.ids[id_disp + 4].name);
		SetDlgItemText(hDlg, CB_NAME6, config.id.ids[id_disp + 5].name);
		SetDlgItemText(hDlg, CB_NAME7, config.id.ids[id_disp + 6].name);
		sprintf(string, "%li", config.id.ids[id_disp + 0].examples);
        SetDlgItemText(hDlg, CB_EX1, string);
		sprintf(string, "%li", config.id.ids[id_disp + 1].examples);
        SetDlgItemText(hDlg, CB_EX2, string);
		sprintf(string, "%li", config.id.ids[id_disp + 2].examples);
        SetDlgItemText(hDlg, CB_EX3, string);
		sprintf(string, "%li", config.id.ids[id_disp + 3].examples);
        SetDlgItemText(hDlg, CB_EX4, string);
		sprintf(string, "%li", config.id.ids[id_disp + 4].examples);
        SetDlgItemText(hDlg, CB_EX5, string);
		sprintf(string, "%li", config.id.ids[id_disp + 5].examples);
        SetDlgItemText(hDlg, CB_EX6, string);
		sprintf(string, "%li", config.id.ids[id_disp + 6].examples);
		SetDlgItemText(hDlg, CB_EX7, string);
		SetFocus(hDlg);
		return FALSE;
		break;

    case WM_SYSCOMMAND:
            // Pass WM_SYSCOMMAND messages on to main window so both
            // main window and dialog box get iconized, minimized etc.
            // in parallel.
		SendMessage(mainhwnd, message, wParam, lParam);
		break;

	case WM_COMMAND:
		switch( wParam) {
			case IDOK:
				EndDialog(hDlg, TRUE);
				break;
			case CB_MORE:
				if (id_disp == config.id.num - 6) id_disp = 1;
				else id_disp += 7;
				if (id_disp > config.id.num - 6) id_disp = config.id.num - 6;
				if (id_disp < 1) id_disp = 1;
				SendMessage(hDlg, WM_PAINT, 0, 0);
                break;
			default:
        		return FALSE;
    	}//End of switch on command
		break;

  		default:
    		return FALSE;
 }  // switch

 return(TRUE);
}  // end of DlgBoxProc()


void cmconbox()
{

	DialogBox(hInst, "CON_BOX", mainhwnd,
		(DLGPROC) DlgBoxProc);

}  
