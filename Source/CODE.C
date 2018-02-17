#include "FUNCTIONS.H"
#include <stdlib.h>
#include <mem.h>
#include <memory.h>
#include <mmsystem.h>
#include <shellapi.h>


#define HALFMEG (unsigned long)(1024.0 * 1024.0 * 0.5)

extern CONFIG_STRUCT config;

codefile(HWND hWnd) {
	if (config.code.on) {
		codenow(hWnd, 0, 0);
	}
    else {
		MessageBox( hWnd, "Coding is OFF","Cannot Train", MB_OK);
	}
}
codeon(HWND hWnd) {
	int on = config.code.on;
	int button;

	if (on) {
		button = MessageBox( hWnd, "Turn Coding Off?","Delete Coding Data", MB_YESNO);
		if (button == IDYES) {
			config.code.on = 0;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			if (config.file.cod.data)
//				farfree(config.file.cod.data);
				free(config.file.cod.data);
			config.file.cod.data = 0;
			config.file.cod.cur_pos = 0;
			config.file.cod.size = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Turn On Coding?","CODING IS OFF", MB_YESNO);
		if (button == IDYES) {
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			if (config.file.cod.data)
//				farfree(config.file.cod.data);
				free(config.file.cod.data);
			config.file.cod.data = 0;
			config.file.cod.cur_pos = 0;
			config.file.cod.size = 0;
		}
    }
}
codeucl(HWND hWnd) {
	int ucl = config.code.ucl;
	int button;

    if (!config.code.on) return(0);
	if (!ucl) {
		button = MessageBox( hWnd, "Use UCL Coding?","Select Coding", MB_YESNO);
		if (button == IDYES) {
			if (!config.file.cod.data)
				if (!(config.file.cod.data =
//					farcalloc(1, HALFMEG))){
					(char*) calloc(1, HALFMEG))){
					MessageBox(hWnd,"Can't Alloc 1/2 MEG!","Coding Allocation",MB_OK);
					return(0);
		    }
			config.code.on = 1;
			config.code.ucl = 1;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			config.file.cod.cur_pos = config.file.cod.data;
			config.file.cod.size = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Turn Off UCL Coding?","Delete Coding Data", MB_YESNO);
		if (button == IDYES) {
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			if (config.file.cod.data)
//				farfree(config.file.cod.data);
				free(config.file.cod.data);
			config.file.cod.data = 0;
			config.file.cod.cur_pos = 0;
			config.file.cod.size = 0;
		}
    }
}
codeautoassc(HWND hWnd) {
	int autoassc = config.code.autoassc;
	int button;

	if (!config.code.on) return(0);
	if (!autoassc) {
		button = MessageBox( hWnd, "Use Autoassociative Coding?","Select Coding", MB_YESNO);
		if (button == IDYES) {
			if (!config.file.cod.data)
				if (!(config.file.cod.data =
//					farcalloc(1, HALFMEG))){
					(char*) calloc(1, HALFMEG))){
					MessageBox(hWnd,"Can't Alloc 1/2 MEG!","Coding Allocation",MB_OK);
					return(0);
		    }
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 1;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			config.file.cod.cur_pos = config.file.cod.data;
			config.file.cod.size = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Turn Off Autoassociative Coding?","Delete Coding Data", MB_YESNO);
		if (button == IDYES) {
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			if (config.file.cod.data)
//				farfree(config.file.cod.data);
				free(config.file.cod.data);
			config.file.cod.data = 0;
			config.file.cod.cur_pos = 0;
			config.file.cod.size = 0;
		}
    }
}
codescl(HWND hWnd) {
	int scl = config.code.scl;
	int button;

	if (!config.code.on) return(0);
	if (!scl) {
		button = MessageBox( hWnd, "Use SCL Coding?","Select Coding", MB_YESNO);
		if (button == IDYES) {
			if (!config.file.cod.data)
				if (!(config.file.cod.data =
//					farcalloc(1, HALFMEG))){
					(char*) calloc(1, HALFMEG))){
					MessageBox(hWnd,"Can't Alloc 1/2 MEG!","Coding Allocation",MB_OK);
					return(0);
		    }
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 1;
			config.code.back = 0;
			config.code.iter = 0;
			config.file.cod.cur_pos = config.file.cod.data;
			config.file.cod.size = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Turn Off SCL Coding?","Delete Coding Data", MB_YESNO);
		if (button == IDYES) {
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			if (config.file.cod.data)
//				farfree(config.file.cod.data);
				free(config.file.cod.data);
			config.file.cod.data = 0;
			config.file.cod.cur_pos = 0;
			config.file.cod.size = 0;
		}
    }
}
codeback(HWND hWnd) {
	int back = config.code.back;
	int button;

	if (!config.code.on) return(0);
	if (!back) {
		button = MessageBox( hWnd, "Use Backprop Coding?","Select Coding", MB_YESNO);
		if (button == IDYES) {
			if (!config.file.cod.data)
				if (!(config.file.cod.data =
//					farcalloc(1, HALFMEG))){
					(char*) calloc(1, HALFMEG))){
					MessageBox(hWnd,"Can't Alloc 1/2 MEG!","Coding Allocation",MB_OK);
					return(0);
		    }
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 1;
			config.code.iter = 0;
			config.file.cod.cur_pos = config.file.cod.data;
			config.file.cod.size = 0;
		}
    }
	else {
		button = MessageBox( hWnd, "Turn Off Backprop Coding?","Delete Coding Data", MB_YESNO);
		if (button == IDYES) {
			config.code.on = 1;
			config.code.ucl = 0;
			config.code.autoassc = 0;
			config.code.scl = 0;
			config.code.back = 0;
			config.code.iter = 0;
			if (config.file.cod.data)
//				farfree(config.file.cod.data);
				free(config.file.cod.data);
			config.file.cod.data = 0;
			config.file.cod.cur_pos = 0;
			config.file.cod.size = 0;
		}
    }
}
