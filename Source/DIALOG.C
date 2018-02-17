#include "FUNCTIONS.H"
#include <math.h>

static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam);
                           



extern HINSTANCE hInst;                        // hInstance of application
HWND      hwnd;                         // hWnd of main window

HANDLE    hdlg;                         // handle of dialog resource

DLGPROC   lpDlgProc;                    // ptr to proc for dialog box
HWND      hDlgBox;                      // handle of dialog box
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
#pragma argsused
static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
                           WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
        case WM_SYSCOMMAND:
            // Pass WM_SYSCOMMAND messages on to main window so both
            // main window and dialog box get iconized, minimized etc.
            // in parallel.
            SendMessage(hwnd, message, wParam, lParam);
            break;

        case WM_COMMAND:
            // regardless of the command (only ID_OK should arrive)
            // we want to exit the dialog
            EndDialog(hDlg, TRUE);
            break;

        default:
            return FALSE;
    }  // switch

    return(TRUE);
}  // end of DlgBoxProc()
//*************************************************************************
// cmAbout -- invoke the "About" dialog.  Its return code is ignored, since
// the About dialog doesn't return anything to the program.
//*************************************************************************
void cmAbout(void)
{
    lpDlgProc = (DLGPROC) MakeProcInstance( (FARPROC) DlgBoxProc, hInst);
    DialogBox(hInst, "About", hwnd, lpDlgProc);
    FreeProcInstance ( (FARPROC) lpDlgProc );
}  // end of cmAbout()
