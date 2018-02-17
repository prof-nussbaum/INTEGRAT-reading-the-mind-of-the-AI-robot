#include "FUNCTIONS.H"
#include <process.h>
#include <time.h> // for srand(clock())
/***  Global Variables  ***/
int number = 0;
char szName[500];
COLORREF crColor;
HFONT hfFont;
BOOL tfFontLoaded;
char infilename[500];
char outfilename[500];
HWND mainhwnd;
HINSTANCE hInst;    // current instance

CONFIG_STRUCT config;
int drawing_scale = 50;


void CMUFileOpen( HWND hWnd )
{
    OPENFILENAME ofnTemp;
    DWORD Errval;   // Error value
    char buf[5];    // Error buffer
    char Errstr[50]="GetOpenFileName returned Error #";
    char szTemp[] = "All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0";

    infilename[0] = 0;
    outfilename[0] = 0;

    ofnTemp.lStructSize = sizeof( OPENFILENAME );
    ofnTemp.hwndOwner = hWnd;   // An invalid hWnd causes non-modality
    ofnTemp.hInstance = 0;
    ofnTemp.lpstrFilter = (LPSTR)szTemp;    // See previous note concerning string
    ofnTemp.lpstrCustomFilter = NULL;
    ofnTemp.nMaxCustFilter = 0;
    ofnTemp.nFilterIndex = 1;
    ofnTemp.lpstrFile = (LPSTR)infilename;  // Stores the result in this variable
    ofnTemp.nMaxFile = sizeof( szName );
    ofnTemp.lpstrFileTitle = NULL;
    ofnTemp.nMaxFileTitle = 0;
    ofnTemp.lpstrInitialDir = NULL;
    ofnTemp.lpstrTitle = "Select or Type Filename";    // Title for dialog
		ofnTemp.Flags = /*OFN_FILEMUSTEXIST |*/ OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    ofnTemp.nFileOffset = 0;
    ofnTemp.nFileExtension = 0;
    ofnTemp.lpstrDefExt = "*";
    ofnTemp.lCustData = NULL;
    ofnTemp.lpfnHook = NULL;
    ofnTemp.lpTemplateName = NULL;

    if(GetOpenFileName( &ofnTemp ) != TRUE)
    {
        Errval=CommDlgExtendedError();
        if(Errval!=0)   // 0 value means user selected Cancel
        {
            sprintf(buf,"%ld",Errval);
            strcat(Errstr,buf);
            MessageBox(hWnd,Errstr,"WARNING",MB_OK|MB_ICONSTOP);
        }

    }
    InvalidateRect( hWnd, NULL, TRUE ); // Repaint to display the new name
}

/*************************************************************************
Using the CHOOSECOLOR structure and the Windows 3.1 API call ChooseColor(),
eases the selection of colors for the programmer and for the user.  The
comments for the File Open dialog regarding the help file and the structure
size also apply to the color dialog.
**************************************************************************/
void CMUColor( HWND hWnd )
{
    CHOOSECOLOR ccTemp;
    COLORREF crTemp[16];    // Important, sometimes unused, array

    ccTemp.lStructSize = sizeof( CHOOSECOLOR );
    ccTemp.hwndOwner = hWnd;
    ccTemp.hInstance = 0;
    ccTemp.rgbResult = crColor; // CC_RGBINIT flag makes this the default color
/*
lpCustColors must be set to a valid array of 16 COLORREF's, even if it
is not used.  If it isn't you will probably fail with a GP fault in
COMMDLG.DLL.
*/
    ccTemp.lpCustColors = crTemp;
    ccTemp.Flags = CC_PREVENTFULLOPEN | CC_RGBINIT;
    ccTemp.lCustData = 0L;
    ccTemp.lpfnHook = NULL;
    ccTemp.lpTemplateName = NULL;
    if( ChooseColor( &ccTemp ) == TRUE ) crColor = ccTemp.rgbResult;
    InvalidateRect( hWnd, NULL, TRUE );
}

/**************************************************************************
Using the CHOOSEFONT structure and the Windows 3.1 API call ChooseFont()
eases the selection of fonts for the programmer and for the user.  The
comments for the File Open dialog regarding the help file and the structure
size also apply to the font dialog.
***************************************************************************/
void CMUFont( HWND hWnd )
{
/*
The variables below are static so that multiple calls to the font dialog will
retain previous user selections.
*/
    static CHOOSEFONT cfTemp;
    static LOGFONT lfTemp;

    if( tfFontLoaded == TRUE )  // cfTemp contains previous selections
    {
        cfTemp.Flags |= CF_INITTOLOGFONTSTRUCT;
        cfTemp.rgbColors = crColor;
    }
    else
    {
        cfTemp.lStructSize = sizeof( CHOOSEFONT );
        cfTemp.hwndOwner = hWnd;
        cfTemp.hDC = 0;
        cfTemp.lpLogFont = &lfTemp; // Store the result here
        cfTemp.Flags = CF_EFFECTS | CF_FORCEFONTEXIST | CF_SCREENFONTS;
        cfTemp.rgbColors = crColor; // Color and font dialogs use the same color
        cfTemp.lCustData = 0L;
        cfTemp.lpfnHook = NULL;
        cfTemp.lpTemplateName = NULL;
        cfTemp.hInstance = 0;
        cfTemp.lpszStyle = NULL;
        cfTemp.nFontType = SCREEN_FONTTYPE;
        cfTemp.nSizeMin = 0;
        cfTemp.nSizeMax = 0;
    }
    if( ChooseFont( &cfTemp ) == TRUE )
    {
        if( tfFontLoaded == TRUE )
            DeleteObject( hfFont );
        crColor = cfTemp.rgbColors;
        hfFont = CreateFontIndirect( &lfTemp );
        tfFontLoaded = TRUE;
    }
    InvalidateRect( hWnd, NULL, TRUE );
}

/**********************************************************************/
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX  wc;	/* A properties struct of our window */

	// Fill in window class structure with parameters that describe the
	// main window.
	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "INTEGRAT";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */
    wc.style = CS_HREDRAW | CS_VREDRAW; // Class style(s).                                                        // windows of this class.
    wc.lpszMenuName = "CMDLGAPMENU";    // Name of menu resource in .RC file.

/*  FROM Windows 3.1 VERSION:
//  wc.lpfnWndProc = (long (FAR PASCAL*)())MainWndProc; // Function to retrieve messages for
    wc.lpfnWndProc = (long (FAR PASCAL*)())WndProc; // Function to retrieve messages for
    wc.cbClsExtra = 0;  // No per-class extra data.
    wc.cbWndExtra = 0;  // No per-window extra data.
    wc.hInstance = hInstance;   // Application that owns the class.
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = "LPC";   // Name used in call to CreateWindow.

    wc.style = CS_HREDRAW | CS_VREDRAW; // Class style(s).                                                        // windows of this class.
    wc.lpszMenuName = "CMDLGAPMENU";    // Name of menu resource in .RC file.

    return (RegisterClass(&wc));
*/
    /* Register the window class and return success/failure code. */
    return (RegisterClassEx(&wc));

}


/************************************************************************/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND    hWnd;   // Main window handle.
	char	*c, *end;

	// Initialize the random number generator
	srand(clock());

	hInst = hInstance;
	/* Save the instance handle in static variable, which will be used in  */
	/* many subsequence calls from this application to Windows.            */
	end = &(config.end);
	for (c = (char *)&config; c < end; c++)
		*c = 0x00;

 	/* Create a main window for this application instance.  */
	hWnd = CreateWindowEx(
 		WS_EX_CLIENTEDGE,
	       	"INTEGRAT",                 // See RegisterClass() call.
		"NN for Phoneme Recognition v 2.0, Thesis MSEE Polytechnic P.Nussbaum 1992",  // Text for window title bar.
	        WS_VISIBLE|WS_OVERLAPPEDWINDOW,          // Window style.
	        CW_USEDEFAULT,             // Default horizontal position.
	        CW_USEDEFAULT,          // Default vertical position.
	        CW_USEDEFAULT,              // Default width.
	        CW_USEDEFAULT,          // Default height.
	        NULL,                        // Overlapped windows have no parent.
	        NULL,                        // Use the window class menu.
	        hInstance,                    // This instance owns this window.
	        NULL                         // Pointer not needed.
	);

/* FROM Windows 3.1 VERSION
	hWnd = CreateWindow(
        	"INTEGRAT",                 // See RegisterClass() call.
		"NN for Phoneme Recognition v 2.0, Thesis MSEE Polytechnic P.Nussbaum 1992",  // Text for window title bar.
	        WS_OVERLAPPEDWINDOW,          // Window style.
	        CW_USEDEFAULT,             // Default horizontal position.
	        CW_USEDEFAULT,          // Default vertical position.
	        CW_USEDEFAULT,              // Default width.
	        CW_USEDEFAULT,          // Default height.
	        NULL,                        // Overlapped windows have no parent.
	        NULL,                        // Use the window class menu.
	        hInstance,                    // This instance owns this window.
	        NULL                         // Pointer not needed.
	);
*/
	/* If window could not be created, return "failure" */

    if (!hWnd)
        return (FALSE);

    /* Make the window visible; update its client area; and return "success" */

    mainhwnd = hWnd;
    ShowWindow(hWnd, nCmdShow); // Show the window
    UpdateWindow(hWnd);         // Sends WM_PAINT message
    return (TRUE);              // Returns the value from PostQuitMessage

}


/****************************************************************************
    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)
****************************************************************************/
//long FAR PASCAL _export MainWndProc(HWND hWnd, unsigned message,
//                                    WORD wParam, LONG lParam)
LRESULT CALLBACK WndProc(HWND hwnd, unsigned int message, WPARAM wParam, LPARAM LParam)
{
    HFONT fTemp;    // Placeholder for the original font
    RECT rTemp;     // Client are needed by DrawText()
    HDC hdc;        // HDC for Window
    PAINTSTRUCT ps; // Paint Struct for BeginPaint call
    char *ilabel = "Input Filename:";
    char *olabel = "Output Filename:";
	char *parse1, *parse2;
    unsigned long x, y;

    switch (message) {
    case WM_CREATE: // Initialize Global vars
            strcpy( szName, "" );           // Empty the file name string
            crColor = RGB( 0, 0, 0 );       // Use black as the default color
            hfFont = 0;                     // Empty the handle to the font
            tfFontLoaded = FALSE;           // Set the font selected flag to false
        return NULL;

	case WM_LBUTTONDOWN:
		x = LOWORD(LParam);
		y = HIWORD(LParam);
		click(hwnd, x, y);
		break;

	case WM_PAINT:
    // Display the file name using the selected font in the selected color.
		   hdc=BeginPaint(hwnd,&ps);
            SetTextColor( hdc, crColor );
            if( tfFontLoaded == TRUE )
                fTemp = (HFONT)SelectObject( hdc, hfFont );
            GetClientRect( hwnd, &rTemp );
//          DrawText( hdc, szName, strlen( szName ), &rTemp, DT_CENTER | DT_WORDBREAK );

//			for (parse1 = infilename, parse2 = outfilename;
//                    (*parse1 != '.') || (*(parse1 - 1) == '\\');
//                    *parse2++ = *parse1++);
//            *parse2++ = '.';
//            *parse2++ = 'L';
//            *parse2++ = 'P';
//            *parse2++ = 'C';
//			*parse2++ = '\0';
/* Use better drawing method that asks rectangle size first*/
//            rTemp.top += DrawText( hdc, ilabel, strlen( ilabel ), &rTemp, DT_CENTER | DT_WORDBREAK );
//            rTemp.top += DrawText( hdc, infilename, strlen( infilename ), &rTemp, DT_CENTER | DT_WORDBREAK );
//            rTemp.top += DrawText( hdc, olabel, strlen( olabel ), &rTemp, DT_CENTER | DT_WORDBREAK );
//            rTemp.top += DrawText( hdc, outfilename, strlen( outfilename ), &rTemp, DT_CENTER | DT_WORDBREAK );
            if( tfFontLoaded == TRUE )
                SelectObject( hdc, fTemp );
			EndPaint(hwnd,&ps);
			drawall(hwnd);
        break;

    case WM_COMMAND:    // message: command from application menu
		switch(wParam)
        {
            case CM_EXIT:
                    DestroyWindow(hwnd);
                break;

            case CM_U_COLOR:
                    CMUColor(hwnd);
                break;

            case CM_U_FONT:
                    CMUFont(hwnd);
				break;
			case CM_U_DRAWNOT:
				drawnot(hwnd);
                break;
			case CM_U_DRAWNOW:
				drawnow(hwnd);
                break;
			case CM_U_DRAWBW:
				drawbw(hwnd);
                break;

            case CM_U_RUN:
				break;

            case CM_U_PLAY:
                mplay(hwnd);
                break;

			 case CM_U_WAV:
				identify_wav_resources(hwnd);
				break;

			case CM_U_HELPABOUT:
				WinHelp(hwnd, "mainin.hlp", HELP_CONTENTS, 0L);
				break;

	case CM_U_FILEOPEN:
		fileopen(hwnd);
// dos only		spawnl( P_WAIT, "D:\\win\\soundrec.exe", NULL);
		break;
	case CM_U_FILECON:
		cmconbox();
		break;
	case CM_U_FILEPLAY:
		filecon(hwnd);
    	break;
	case CM_U_FILEREC:
		filerec(hwnd);
		break;
	case CM_U_FILERATE:
		playrate(hwnd);
		break;
	case CM_U_SEGLEV:
		seglev(hwnd);
    	break;
	case CM_U_SEGDUR:
		segdur(hwnd);
    	break;
	case CM_U_SEGMIN:
		segmin(hwnd);
    	break;
	case CM_U_SEGMAX:
		segmax(hwnd);
		break;
	case CM_U_IDNEXT:
		idnext(hwnd);
    	break;
	case CM_U_IDNUM:
		idnum(hwnd);
    	break;
	case CM_U_IDPAIR:
		idpair(hwnd);
    	break;
	case CM_U_IDSTEP:
		idstep(hwnd);
    	break;
	case CM_U_IDFILE:
		idfile(hwnd);
    	break;
	case CM_U_TRANON:
		tranon(hwnd);
    	break;
	case CM_U_TRANLPC:
		tranlpc(hwnd);
    	break;
	case CM_U_TRANFFT:
		tranfft(hwnd);
    	break;
	case CM_U_TRANRMS:
		tranrms(hwnd);
    	break;
	case CM_U_TRANSIZE:
		transize(hwnd);
    	break;
	case CM_U_TRANINTERVAL:
		traninterval(hwnd);
    	break;
	case CM_U_TRANDIM:
		trandim(hwnd);
    	break;
	case CM_U_TRANNOPITCH:
		trannopitch(hwnd);
    	break;
	case CM_U_TRANHAMM:
		tranhamm(hwnd);
    	break;
	case CM_U_UNTRCALC:
		untrcalc(hwnd);
    	break;
	case CM_U_UNTRFILE:
		untrfile(hwnd);
    	break;
	case CM_U_UNTRSAVE:
		untrsave(hwnd);
    	break;
	case CM_U_UNTRPLAY:
		untrplay(hwnd);
    	break;
	case CM_U_CODEON:
		codeon(hwnd);
    	break;
	case CM_U_CODEFILE:
		codefile(hwnd);
    	break;
	case CM_U_CODEUCL:
		codeucl(hwnd);
    	break;
	case CM_U_CODEAUTOASSC:
		codeautoassc(hwnd);
    	break;
	case CM_U_CODESCL:
		codescl(hwnd);
    	break;
	case CM_U_CODEBACK:
		codeback(hwnd);
    	break;
	case CM_U_DECOON:
		decode(hwnd);
		break;
	case CM_U_SIZE:
		cmresizebox(&drawing_scale);
		break;
			default:
                break;
        }
        break;

    case WM_QUIT:
    case WM_DESTROY:    // message: window being destroyed
            if( hfFont != 0 )
				DeleteObject( hfFont );
			WinHelp(hwnd, "mainin.hlp", HELP_QUIT, NULL);
            PostQuitMessage(0);
        break;
	default:            // Passes it on if unproccessed
	    return (DefWindowProc(hwnd, message, wParam, LParam));
	}
    return (NULL);
}

#pragma argsused
/**************************************************************/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;            // message
    if (!hPrevInstance) // Other instances of app running?
	    if (!InitApplication(hInstance))    // Initialize shared things
        	return (FALSE); // Exits if unable to initialize

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while (GetMessage(&msg, // message structure
        NULL,   // handle of window receiving the message
        NULL,   // lowest message to examine
        NULL))  // highest message to examine
    {
	    TranslateMessage(&msg); // Translates virtual key codes
	    DispatchMessage(&msg);  // Dispatches message to window
    }
    return (msg.wParam);    // Returns the value from PostQuitMessage
}


