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
static int      gsize, ginterval, gdim;

static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam)
{

	char *dummy;

    switch (message) {
	case WM_CREATE:
    case WM_PAINT:
		SetDlgItemInt(hDlg, TB_SIZE, gsize, TRUE);
		SetDlgItemInt(hDlg, TB_INTERVAL, ginterval, TRUE);
		SetDlgItemInt(hDlg, TB_DIM, gdim, TRUE);
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
			case TB_ACC:
				gsize = (int)GetDlgItemInt( hDlg, TB_SIZE, &truth, TRUE);
				ginterval = (int)GetDlgItemInt( hDlg, TB_INTERVAL, &truth, TRUE);
				gdim = (int)GetDlgItemInt( hDlg, TB_DIM, &truth, TRUE);
				EndDialog(hDlg, TRUE);
				break;
			case TB_CAN:
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

void cmtranbox(int *size, int *interval, int *dim)
{

	gsize = *size;
	ginterval = *interval;
    gdim = *dim;
	DialogBox(hInst, "TRAN_BOX", mainhwnd,
		(DLGPROC) DlgBoxProc);
	*size = gsize;
	*interval = ginterval;
	*dim = gdim;

}  // end of user I/O
