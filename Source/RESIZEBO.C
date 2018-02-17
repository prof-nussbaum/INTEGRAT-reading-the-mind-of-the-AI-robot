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
static int      gscale;


static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam)
{

	char *dummy;

    switch (message) {
	case WM_CREATE:
    case WM_PAINT:
		SetDlgItemInt(hDlg, RB_SCALE, gscale, TRUE);
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
			case IDOK:
				gscale = (int)GetDlgItemInt( hDlg, RB_SCALE, &truth, TRUE);
				EndDialog(hDlg, TRUE);
				break;
			case IDCANCEL:
				EndDialog(hDlg, TRUE);
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


void cmresizebox(int *scale)
{

	gscale = *scale;
	DialogBox(hInst, "RESIZE_BOX", mainhwnd,
		(DLGPROC) DlgBoxProc);
    *scale = gscale;

}  // end of user I/O
