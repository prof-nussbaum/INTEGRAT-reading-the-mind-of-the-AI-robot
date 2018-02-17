#include "FUNCTIONS.H"
#include <stdlib.h>
#include <math.h>

extern HINSTANCE hInst;                        // hInstance of application
extern HWND      mainhwnd;                     // hWnd of main window

extern HANDLE    hdlg;                         // handle of dialog resource

static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam);
static DLGPROC  lpDlgProc;                    // ptr to proc for dialog box
static BOOL 	truth;
static int      gnum;
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

	char *dummy;

    switch (message) {
	case WM_CREATE:
    case WM_PAINT:
		SetDlgItemInt(hDlg, ID2_NUM, gnum, TRUE);
		SendDlgItemMessage(hDlg, ID2_NUM, EM_SETSEL, 0, MAKELONG(0, -1));
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
				gnum = (int)GetDlgItemInt( hDlg, ID2_NUM, (BOOL far *)&truth, TRUE);
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
//*************************************************************************
// cmgetnum -- invoke the "getnum" dialog.  Its return code is ignored, since
// the getnum dialog doesn't return anything to the program.
//*************************************************************************
void cmid2box(int *num)
{

	gnum = *num;
	DialogBox(hInst, "ID2_BOX", mainhwnd,
		(DLGPROC) DlgBoxProc);
	*num = gnum;

}  // end of user I/O
