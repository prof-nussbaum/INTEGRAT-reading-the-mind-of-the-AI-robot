#include "FUNCTIONS.H"
#include <stdlib.h>
#include <math.h>

extern HINSTANCE hInst;                        // hInstance of application
extern HWND      mainhwnd;                     // hWnd of main window

extern HANDLE    hdlg;                         // handle of dialog resource

static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam);
static DLGPROC  lpDlgProc;                    // ptr to proc for dialog box
static char 	gname[255];
static BOOL 	truth;
static int      gnum;

static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam)
{

    switch (message) {
	case WM_CREATE:
    case WM_PAINT:
		SetDlgItemText(hDlg, IB_NAME, gname);
		SetDlgItemInt(hDlg, IB_NUM, gnum, TRUE);
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
			case IB_ACC:
				gnum = (int)GetDlgItemInt( hDlg, IB_NUM, (BOOL far *)&truth, TRUE);
				GetDlgItemText( hDlg, IB_NAME, gname, 200);
				EndDialog(hDlg, TRUE);
				break;
			case IB_CAN:
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

void cmidbox(int *num, char *name)
{

	strcpy(gname, name);
    gnum = *num;
	DialogBox(hInst, "ID_BOX", mainhwnd,
		(DLGPROC) DlgBoxProc);
	gname[9] = 0;//force termination
	strcpy(name, gname);
	*num = gnum;

}  // end of user I/O
numtoname() {
	gname[0] = 'A' + gnum;
}
nametonum() {
	gnum = gname[0];
}
