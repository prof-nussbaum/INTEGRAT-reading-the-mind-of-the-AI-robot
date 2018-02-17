#include "FUNCTIONS.H"
#include <mmsystem.h>

/***  Global Variables  ***/
extern char szName[256];
extern COLORREF crColor;
extern HFONT hfFont;
extern BOOL tfFontLoaded;
extern HINSTANCE hInst;    // current instance
extern char infilename[255];
extern char outfilename[255];

char message[120];


identify_wav_resources(HWND hWnd)
{
    HFONT fTemp;    // Placeholder for the original font
    RECT rTemp;     // Client are needed by DrawText()
    HDC hdc;        // HDC for Window
    PAINTSTRUCT ps; // Paint Struct for BeginPaint call
    char *ilabel = "Input Filename:\n";
    char *olabel = "Output Filename:\n";
    char *parse1, *parse2;
    unsigned int inumdev, onumdev, x;
    WAVEINCAPS icaps;
    WAVEOUTCAPS ocaps;
    HWAVEIN ihandle;
    HWAVEOUT ohandle;
    WAVEFORMAT iformat;


    hdc = GetDC(hWnd);
    SetTextColor( hdc, crColor );
    if( tfFontLoaded == TRUE )
        fTemp = (HFONT)SelectObject( hdc, hfFont );
    GetClientRect( hWnd, (RECT FAR*)&rTemp );
    sprintf(message,"WAV Device Info:");
    DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT | DT_CALCRECT );
    rTemp.top += DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT);
    inumdev = waveInGetNumDevs();
    sprintf(message,"Number of Input WAV Devices: %d", inumdev);
    DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT | DT_CALCRECT );
    rTemp.top += DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT);
    for (x = 0; x < inumdev; x++) {
        waveInGetDevCaps(x, &icaps, sizeof(icaps));
        sprintf(message, "%d - wMid:%d, wPid:%d, Ver:%d, Fmts:%d, Chans:%d, %s",
            x, icaps.wMid, icaps.wPid, icaps.vDriverVersion,
            icaps.dwFormats, icaps.wChannels, icaps.szPname);
        DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT | DT_CALCRECT );
        rTemp.top += DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT);
    }
    onumdev = waveOutGetNumDevs();
    sprintf(message,"Number of Output WAV Devices: %d", inumdev);
    DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT | DT_CALCRECT );
    rTemp.top += DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT);
    for (x = 0; x < onumdev; x++) {
        waveOutGetDevCaps(x, &ocaps, sizeof(ocaps));
        sprintf(message, "%d - wMid:%d, wPid:%d, Ver:%d, Fmts:%d, Chans:%d, Supp:%d, %s",
            x, ocaps.wMid, ocaps.wPid, ocaps.vDriverVersion,
            ocaps.dwFormats, ocaps.wChannels, ocaps.dwSupport, ocaps.szPname);
        DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT | DT_CALCRECT );
        rTemp.top += DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT);
	}
    /*
    sprintf(message,"Testing Waveform Devices...");
    DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT | DT_CALCRECT );
    rTemp.top += DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT);
    iformat.wFormatTag = 0;
    iformat.nChannels = 1;
    iformat.nSamplesPerSec = 11000;
    iformat.nAvgBytesPerSec = 11000;
    iformat.nBlockAlign = 11000;
    x = waveInOpen((LPHWAVEIN)&ihandle,//will put handle here
                    0,                  //0th oinput device
                    (LPWAVEFORMAT)&iformat, //format
                    (DWORD)hWnd,                //callback func. (use window)
                    0,              //instance (this instance)
                    WAVE_ALLOWSYNC | CALLBACK_WINDOW);
    sprintf(message,"Synch Input (record) returns %d", x);
    DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT | DT_CALCRECT );
    rTemp.top += DrawText( hdc, message, -1, (RECT FAR*)&rTemp, DT_LEFT);
	*/
    if( tfFontLoaded == TRUE )
        SelectObject( hdc, fTemp );
    ReleaseDC(hWnd, hdc);
}
