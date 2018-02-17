#include "FUNCTIONS.H"
#include <memory.h>
#include <mem.h>

extern CONFIG_STRUCT config;
extern SEGMENT *active_segment;
extern HINSTANCE hInst;
static	HGLOBAL hglob;
static	char *voice;
static  SEGMENT fake_seg;
static  SEGMENT last_segment;
static char ram_flag = 0;

#define HALFMEG (unsigned long)(1024.0 * 1024.0 * 0.5)

/*
playseg(HWND hWnd, SEGMENT *segptr);
char *fftuntran(float *tran, int num_trans,
		  char 	*voice, TRAN_STRUCT *dims);
char *lpcuntran(float *tran, int num_trans,
		  char 	*voice, TRAN_STRUCT *dims);
float unnorm_dim(float x, int y);
float norm_dim(float x, int y);
*/


untrcalc(HWND hWnd) {untrplay(hWnd);}
untrfile(HWND hWnd) {untrplay(hWnd);}
untrsave(HWND hWnd) {untrplay(hWnd);}
untrplay(HWND hWnd) {
	SEGMENT *segptr = config.seg.segments;
	SEGMENT *end = &config.seg.segments[127];

	for (;segptr <= end; segptr++) {
		if ((segptr->tran_start) && (segptr->num_trans)) {
			active_segment = segptr;
			drawseg(hWnd);
			if (!untrseg(hWnd, segptr))
				return FALSE;
		}
	}
}
untrseg(HWND hWnd, SEGMENT *segptr) {
	HCURSOR hcurSave;

    /* check if something is screwy */
	if ((!config.tran.on) ||
		(!config.tran.dim) ||
		(!config.tran.interval) ||
		(!config.tran.size)) {
		return FALSE;
	}

	if ((last_segment.tran_start == segptr->tran_start) &&
		(last_segment.start == segptr->start) &&
		(last_segment.num_trans == segptr->num_trans)) {
		if (fake_seg.end > fake_seg.start) {
			playseg(hWnd, &fake_seg);
	    }
		return TRUE;
	}
	if (ram_flag) {
		GlobalUnlock(hglob);
	    GlobalFree(hglob);
		hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, HALFMEG);
		if (!(voice = GlobalLock(hglob))) {
			MessageBox (hWnd, "Cannot Untransform", "UNTRANSFORM OUT OF RAM", MB_OK);
			return FALSE;
		}
		ram_flag = 1;
		last_segment.tran_start = segptr->tran_start;
		last_segment.start = segptr->start;
		last_segment.num_trans = segptr->num_trans;
	}
	else if (!ram_flag) {
		hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, HALFMEG);
		if (!(voice = GlobalLock(hglob))) {
			MessageBox (hWnd, "Cannot Untransform", "UNTRANSFORM OUT OF RAM", MB_OK);
			return FALSE;
        }
		ram_flag = 1;
		last_segment.tran_start = segptr->tran_start;
		last_segment.start = segptr->start;
		last_segment.num_trans = segptr->num_trans;
	}

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst,"CUR_TRAN" ));
	if (config.tran.lpc) {
		fake_seg.end = lpcuntran(segptr->tran_start, segptr->num_trans,
						(char *)voice, &config.tran);
		fake_seg.start = voice;
	}
    else {/*FFT Assumed */
		fake_seg.end = fftuntran(segptr->tran_start, segptr->num_trans,
						(char *)voice, &config.tran);
		fake_seg.start = voice;
	}
	SetCursor(hcurSave);
    if (fake_seg.end > fake_seg.start) {
		playseg(hWnd, &fake_seg);
    }
	return TRUE;
}
decode(HWND hWnd) {
	HCURSOR hcurSave;
	int i, j, k, num_tran;
	float far *sample;
    float *code;

    num_tran = 10;
	/* check if something is screwy */
	if ((!config.tran.on) ||
		(!config.tran.dim) ||
		(!config.tran.interval) ||
		(!config.tran.size) ||
		(!config.code.on) ||
		(!config.code.iter)) {
		return FALSE;
	}
	/* bug, can't decode LPC because causes system to lockup (underflow it seems*/
	if (config.tran.lpc) return(FALSE);

	hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, HALFMEG);
	if (!(voice = GlobalLock(hglob))) {
		MessageBox (hWnd, "Cannot Decode", "DECODE OUT OF RAM", MB_OK);
		return FALSE;
    }

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst,"CUR_TRAN" ));
	code = (float *)config.file.cod.data;
	for (i = 0; i < config.id.num; i++) {
		sample = (float *)voice;
		fake_seg.tran_start = code;
		for(j = 0; j < num_tran; j++) {
			for (k = 0; k < config.tran.dim; k++){
				if (!k)
                	if (*sample < 15)
						*sample = 15;//minimum RMS
				*sample = unnorm_dim(*code, k);
				sample++;
				code++;
			}
            code = fake_seg.tran_start;
		}
        fake_seg.tran_start = (float *)voice;
		if (config.tran.lpc) {
			fake_seg.end = lpcuntran(fake_seg.tran_start, num_tran,
							(char *)sample, &config.tran);
			fake_seg.start = (char *)sample;
		}
	    else {/*FFT Assumed */
			fake_seg.end = fftuntran(fake_seg.tran_start, num_tran,
							(char *)sample, &config.tran);
			fake_seg.start = (char*)sample;
		}

		if (fake_seg.end > fake_seg.start) {
			playseg(hWnd, &fake_seg);
		}
        code += config.tran.dim;
    }
	SetCursor(hcurSave);
	GlobalUnlock(hglob);
	GlobalFree(hglob);
	return TRUE;
}

