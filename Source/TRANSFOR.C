#include "FUNCTIONS.H"
#include <memory.h>
#include <mem.h>
extern CONFIG_STRUCT config;
extern SEGMENT *active_segment;
extern HINSTANCE hInst;

static MEMORY_FILE workspace;
#define HALFMEG (unsigned long)(1024.0 * 1024.0 * 0.5)
/*
float *ffttran(char *start,
		char *end,
		float *transform,
		int *num_transforms,
		TRAN_STRUCT *dimensions);
float *lpctran(char *start,
		char *end,
		float *transform,
		int *num_transforms,
		TRAN_STRUCT *dimensions);
*/
/* all of these functions are the same */
transize(HWND hWnd) {
	tranchange(hWnd);
}
traninterval(HWND hWnd) {
	tranchange(hWnd);
}
trandim(HWND hWnd) {
	tranchange(hWnd);
}
tranon(HWND hWnd) {
	transelect(hWnd);
}
tranlpc(HWND hWnd) {
	transelect(hWnd);
}
tranfft(HWND hWnd) {
	transelect(hWnd);
}
trannopitch(HWND hWnd) {
	int pitch = config.tran.nopitch;
	int button;

	if (pitch) {
		button = MessageBox( hWnd, "Allow Pitch Unfiltered?","PITCH FILTER IN USE", MB_YESNO);
		if (button == IDYES) {
			config.tran.nopitch = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Filter Out Pitch?","NO PITCH FILTER IN USE", MB_YESNO);
		if (button == IDYES) {
			config.tran.nopitch = 1;
		}
    }
}
tranhamm(HWND hWnd) {
	int hamm = config.tran.hamm;
	int button;

	if (hamm) {
		button = MessageBox( hWnd, "Use Rectangular Window?","HAMMING WINDOW IN USE", MB_YESNO);
		if (button == IDYES) {
			config.tran.hamm = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Use Hamming Window?","RECTANGULAR WINDOW IN USE", MB_YESNO);
		if (button == IDYES) {
			config.tran.hamm = 1;
		}
    }
}

tranrms(HWND hWnd) {
	int rms = config.tran.rms;
	int button;

	if (rms) {
		button = MessageBox( hWnd, "Use COS Only (phase info)?","RMS (no phase) IN USE", MB_YESNO);
		if (button == IDYES) {
			config.tran.rms = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Use RMS (no phase info)?","COS ONLY (phase) IN USE", MB_YESNO);
		if (button == IDYES) {
			config.tran.rms = 1;
		}
    }
}
transelect(HWND hWnd) {
	int on = config.tran.on;
	int lpc = config.tran.lpc;
    int fft = config.tran.fft;
	int button;

	if (on) {
		button = MessageBox (hWnd, "Turn Tranform Off?", "TRANSFORM ON", MB_YESNO);
		if (button == IDYES) {
			config.tran.on = 0;
			config.tran.lpc = 0;
            config.tran.fft = 0;
//			tranclear();
			tranclear(hWnd);
		}
	}
	else {
		button = MessageBox (hWnd, "Turn Tranform On?", "TRANSFORM OFF", MB_YESNO);
		if (button == IDYES) {
			config.tran.on = 1;
			button = MessageBox (hWnd, "Use LPC Transform?", "CHOOSE TRANSFORM", MB_YESNO);
			if (button == IDYES) {
				config.tran.lpc = 1;
                config.tran.fft = 0;
				MessageBox (hWnd, "Using LPC Transform", "LPC TRANSFORM", MB_OK);
			}
			else {
				config.tran.lpc = 0;
				config.tran.fft = 1;
				config.tran.rms = 1;
				MessageBox (hWnd, "Using FFT Transform (RMS)", "FFT TRANSFORM", MB_OK);
			}
//			tranall();
			tranall(hWnd);
		}
	}
}
tranchange(HWND hWnd){
	int size = config.tran.size;
	int interval = config.tran.interval;
	int dim = config.tran.dim;

	cmtranbox(&config.tran.size, &config.tran.interval, &config.tran.dim);
	if ((size != config.tran.size) ||
		(interval != config.tran.interval) ||
		(dim != config.tran.dim)) {
		tranall(hWnd);
	}
}
tranall(HWND hWnd) {
	SEGMENT *segptr = config.seg.segments;
	SEGMENT *end = &config.seg.segments[127];

    tranclear(hWnd);
//	if (!(workspace.data = farcalloc(1, HALFMEG))) {
	if (!(workspace.data = (char *) calloc(1, HALFMEG))) {
		MessageBox (hWnd, "Cannot Allocate Memory", "TRANSFORM FAILED", MB_OK);
		return FALSE;
	}
	else {
		workspace.cur_pos = workspace.data;
		workspace.size = 0;
    }
	for (;segptr <= end; segptr++) {
		if (segptr->start) {
			if (!transeg(hWnd, segptr))
				return FALSE;
		}
	}
}
tranclear(HWND hWnd) {
	SEGMENT *segptr = config.seg.segments;
	SEGMENT *end = &config.seg.segments[127];

	for (;segptr <= end; segptr++) {
		segptr->num_trans = 0;
		segptr->tran_start = 0;
	}
	if (workspace.data) {
//		farfree(workspace.data);
		free(workspace.data);
		workspace.size = 0;
		workspace.cur_pos = 0;
		workspace.data = 0;
	}
    drawtran(hWnd);
}
transeg(HWND hWnd, SEGMENT *segptr) {
	char *dstart, *dend;
	float *tran;
	int num_trans;
	HCURSOR hcurSave;
    float frame_size = 4.0 * config.tran.dim;

	if (!workspace.data) {
//		if (!(workspace.data = farcalloc(1, HALFMEG))) {
		if (!(workspace.data = (char *) calloc(1, HALFMEG))) {
			MessageBox (hWnd, "Cannot Allocate Memory", "TRANSFORM FAILED", MB_OK);
			return FALSE;
		}
		else {
			workspace.cur_pos = workspace.data;
			workspace.size = 0;
		}
	}
	dstart = segptr->start;
	dend = segptr->end;
	tran = (float *)workspace.cur_pos;

	if ((!config.tran.on) ||
		(!workspace.data) ||
		(!config.tran.dim) ||
		(!config.tran.interval) ||
		(!config.tran.size)) {
		segptr->num_trans = 0;
		segptr->tran_start = 0;
		return FALSE;
	}
    frame_size = frame_size * ((segptr->end - segptr->start)/config.tran.interval);
	if (workspace.cur_pos - workspace.data >= HALFMEG - frame_size) {
		MessageBox (hWnd, "Reusing Transform RAM", "TRANSFORM GARBAGE COLLECTION", MB_OK);
		workspace.cur_pos = workspace.data;
		workspace.size = 0;
		tran = (float *)workspace.cur_pos;
		return FALSE;
	}

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst,"CUR_TRAN" ));
	if (config.tran.lpc) {
		tran = lpctran(dstart, dend, tran, &num_trans,
			&config.tran);
	}
    else {/*FFT Assumed */
		tran = ffttran(dstart, dend, tran,
			&num_trans, &config.tran);
	}
    if (tran) {
		segptr->num_trans = num_trans;
		segptr->tran_start = (float *)workspace.cur_pos;
		workspace.cur_pos = (char *)tran;
		workspace.size += workspace.cur_pos - workspace.data;
		drawonetran(hWnd, segptr);
    }
	SetCursor(hcurSave);
	return TRUE;
}

