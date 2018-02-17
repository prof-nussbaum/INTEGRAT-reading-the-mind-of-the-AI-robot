#include "FUNCTIONS.H"
// STATIC functions for just this module
static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam);

#include <stdlib.h>
#include <math.h>

extern HINSTANCE hInst;                        // hInstance of application
extern HWND      mainhwnd;                     // hWnd of main window

extern HANDLE    hdlg;                         // handle of dialog resource

char globtitle[100], string[34];
int value;


static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam)
{

		char *dummy;

    switch (message) {
	case WM_CREATE:
        case WM_PAINT:
		string[0] = 0;
        	itoa(value, string, 10);
		SetDlgItemText(hDlg, GETNUM_NUM, string);
		SetDlgItemText(hDlg, GETNUM_TEXT, globtitle);
		return FALSE;

        case WM_SYSCOMMAND:
            // Pass WM_SYSCOMMAND messages on to main window so both
            // main window and dialog box get iconized, minimized etc.
            // in parallel.
		SendMessage(mainhwnd, message, wParam, lParam);
        	break;

	case WM_COMMAND:
		switch( wParam) {
			case GETNUM_OK:
//				value = (int)GetDlgItemInt( hDlg, GETNUM_NUM, string, TRUE);
				value = (int)GetDlgItemInt( hDlg, GETNUM_NUM, NULL, TRUE);
				EndDialog(hDlg, TRUE);
				break;
			case GETNUM_CAN:
				EndDialog(hDlg, TRUE);
				break;
			case GETNUM_NUM:
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


void cmgetnum(char *title, int *x)
{
	DLGPROC   lpDlgProc;                    // ptr to proc for dialog box

	value = *x;
	strcpy(globtitle, title);
	DialogBox(hInst, "GETNUM", mainhwnd,
		(DLGPROC) DlgBoxProc);
 	*x = value;

}  // end of cmgetnum()
