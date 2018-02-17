#include "FUNCTIONS.H"
#include <stdlib.h>
#include <math.h>
extern HINSTANCE hInst;                        // hInstance of application
extern HWND      mainhwnd;                     // hWnd of main window

extern HANDLE    hdlg;                         // handle of dialog resource

static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam);
						   static DLGPROC   lpDlgProc;                    // ptr to proc for dialog box
static BOOL		truth;
static int      gthresh, gdur, gmin, gmax;
static char changes;


static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam)
{

	char *dummy;

    switch (message) {
	case WM_CREATE:
    case WM_PAINT:
		SetDlgItemInt(hDlg, SB_THRESH, gthresh, TRUE);
		SetDlgItemInt(hDlg, SB_DUR, gdur, TRUE);
		SetDlgItemInt(hDlg, SB_MIN, gmin, TRUE);
		SetDlgItemInt(hDlg, SB_MAX, gmax, TRUE);
    	SendDlgItemMessage(hDlg, SB_THRESH, EM_SETSEL, 0, MAKELONG(0, -1));
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
			case SB_ACC:
				gthresh = (int)GetDlgItemInt( hDlg, SB_THRESH, &truth, TRUE);
				gdur = (int)GetDlgItemInt( hDlg, SB_DUR, &truth, TRUE);
				gmin = (int)GetDlgItemInt( hDlg, SB_MIN, &truth, TRUE);
				gmax = (int)GetDlgItemInt( hDlg, SB_MAX, &truth, TRUE);
				EndDialog(hDlg, TRUE);
                changes = TRUE;
				break;
			case SB_CAN:
				EndDialog(hDlg, TRUE);
                changes = FALSE;
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


void cmsegbox(int *thresh, int *dur,int *min,int *max)
{

	gthresh = *thresh;
	gdur = *dur;
	gmin = *min;
	gmax = *max;
	DialogBox(hInst, "SEG_BOX", mainhwnd,
		(DLGPROC) DlgBoxProc);
    if (changes) {
		*thresh = gthresh;
		*dur = gdur;
		*min = gmin;
		*max = gmax;
		autosegment(mainhwnd);
	}

}  // end of user I/O
