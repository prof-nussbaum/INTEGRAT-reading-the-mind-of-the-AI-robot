#include "FUNCTIONS.H"
extern CONFIG_STRUCT config;
extern HINSTANCE hInst;

static char active = 0;
static SEGMENT cursor;
SEGMENT *active_segment = 0;

int drawseg(HWND hwnd);
int drawsegedit(HWND hwnd, unsigned int x);
int clearsegedit(HWND hwnd);
int inwavwin(unsigned int x, unsigned int y);
int insegwin(unsigned int x, unsigned int y);
char *getwavptr(unsigned int x);
int codenow(HWND hWnd, int test, SEGMENT *seg);

/* menu entry functions */
seglev(HWND hWnd) {cmsegbox(&config.seg.lev, &config.seg.dur, &config.seg.min, &config.seg.max);}
segdur(HWND hWnd) {cmsegbox(&config.seg.lev, &config.seg.dur, &config.seg.min, &config.seg.max);}
segmin(HWND hWnd) {cmsegbox(&config.seg.lev, &config.seg.dur, &config.seg.min, &config.seg.max);}
segmax(HWND hWnd) {cmsegbox(&config.seg.lev, &config.seg.dur, &config.seg.min, &config.seg.max);}
/* User method of selecting segments */
/* click once on segment window to delete those segments */
/* click twice (start/end) on voice window to add range*/
click(HWND hWnd, unsigned long x,unsigned long y) {
	SEGMENT *segptr, *end;

	/* test if in allowable range */
	if (insegwin((unsigned int)x, (unsigned int)y))
    	delseg(hWnd, x, y);
	else if (inwavwin((unsigned int)x,(unsigned int)y))
		addseg(hWnd, x, y);
}
/* click on voice window to add segment */
addseg(HWND hWnd, unsigned long x,unsigned long y) {

	SEGMENT *segptr, *end;

	if (!active) {
		cursor.start = getwavptr((unsigned int)x);
		cursor.end = 0;
		active = 1;
		drawsegedit(hWnd, (unsigned int)x);
		return TRUE;
	}
	else {
		cursor.end = getwavptr((unsigned int)x);
		if (cursor.end <= cursor.start) {
			MessageBox(hWnd,"Invalid Segment Selected","SEGMENT NO GOOD",MB_OK);
			active = 0;
			cursor.start = 0;
            clearsegedit(hWnd);
			return FALSE;
		}
		/* if automatic segment range is set, use min/max */
		if ((config.seg.min) && (config.seg.max)) {
			if (cursor.end - cursor.start < config.seg.min)
				cursor.end = cursor.start + config.seg.min;
			if (cursor.end - cursor.start > config.seg.max)
				cursor.end = cursor.start + config.seg.max;
			if (cursor.end > config.file.open.data + config.file.open.size) {
				MessageBox(hWnd,"Invalid Segment Selected","SEGMENT NO GOOD",MB_OK);
				active = 0;
				cursor.start = 0;
	            clearsegedit(hWnd);
				return FALSE;
			}
		}
		end = &config.seg.segments[127];
		for (segptr = &config.seg.segments[0];
				segptr <= end; segptr++) {
			if (!segptr->start) break;
        }
		if (segptr >= end) {
			MessageBox(hWnd,"Sorry, All Segments Used Up!","NO MORE SEGMENTS",MB_OK);
			active = 0;
			cursor.start = 0;
			clearsegedit(hWnd);
			return FALSE;
		}
		segptr->start = cursor.start;
		segptr->end = cursor.end;
		// These next 2 lines just for debugging
		//segptr->class1 = 1;
		//segptr->class2 = 2;

		active = 0;
		cursor.start = 0;
		clearsegedit(hWnd);
		//make this the active segment
		active_segment = segptr;
        //transform as necessary
        transeg(hWnd, segptr);
	}
	if (config.code.on) { /*if a coding set is available */
		codenow(hWnd, 1, segptr);
	}
	drawseg(hWnd);
}
/* click on segment window to delete segment */
delseg(HWND hWnd, unsigned long x,unsigned long y) {

	SEGMENT *segptr, *end;
    char *loc;
	int num_found = 0;

	end = &config.seg.segments[127];
	loc = getwavptr((unsigned int)x);
	for (segptr = config.seg.segments;
			segptr <= end; segptr++) {
		if ((loc >= segptr->start) && (loc <= segptr->end))
			num_found += 1;
    }
	if (!num_found) {
		MessageBox(hWnd,"No Segment Selected","CANNOT DELETE",MB_OK);
		return FALSE;
	}
	if (num_found > 1) {
		num_found = MessageBox (hWnd, "Delete More Than 1 Segment?", "MULTIPLE SEGMENT DELETE", MB_YESNO);
		if (num_found == IDNO) {
			MessageBox(hWnd,"No Segments Deleted","DELETE ABORTED",MB_OK);
			return FALSE;
		}
    }
	for (segptr = config.seg.segments;
			segptr <= end; segptr++) {
		if ((loc >= segptr->start) && (loc <= segptr->end)) {
			segptr->start = 0;
			segptr->end = 0;
			segptr->class1 = 0;
			segptr->class2 = 0;
			segptr->num_trans = 0;
			segptr->tran_start = 0;
		}
	}
	active_segment = 0;
	for(segptr = config.seg.segments; segptr <= end; segptr++){
		if (segptr->start) {
			active_segment = segptr;
			break;
		}
    }
	drawseg(hWnd);
}
/* Automatic method of selecting segments */
autosegment(HWND hwnd)
{
	//minimum amplitude to constitute non-silence THRESH
	//minumum samples of non-silence to constitute segment DUR
	//minimum duration of solence to constitute gap MIN
    //maximum length of a segment MAX
	int thresh, dur, min, max;
	char *ptr, *end;
	int silence_count, voice_count, seg_count, cur_size;
	SEGMENT *segptr, *segend, *curseg;
	HCURSOR hcurSave;

    //if nothing to do, return
	if (((max = config.seg.max) == 0) ||
    	(!config.file.open.size)){
		return FALSE;
    }
	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst, "CUR_SEG"));
	// setup variables
	dur 	= config.seg.dur;
	min 	= config.seg.min;
	thresh 	= config.seg.lev;
	segptr	= config.seg.segments;
	segend	= &config.seg.segments[127];
    // skip WAV header info
	ptr 	= config.file.open.data + 100;
	end 	= config.file.open.data + config.file.open.size;
	silence_count = 0;
	voice_count = 0;
	seg_count = 0;
    cur_size = 0;
	//clear all existing segments
	clearseg();
	while ((ptr < end) && (seg_count < 127)){
    	//look for a start of segment
		while ((!segptr->start) && (ptr < end)) {
			if ((*ptr > thresh) || (-*ptr > thresh)) {
				segptr->start = ptr++;
				voice_count = 1;
                silence_count = 0;
			}
			else {
				ptr++;
			}
		}
		if (ptr >= end) {
			segptr->start = 0;
			break;
        }
		//look for an end of segment
		while ((ptr < end) && (voice_count < max)){
			if ((*ptr > thresh) || (-*ptr > thresh)) {
				voice_count += 1;
                silence_count = 0;
				ptr++;
			}
			else {
				silence_count += 1;
                voice_count += 1;
				ptr++;
				if (silence_count > dur) {
					break;
				}
			}
		}
		//see if there was enough voice for a segment
		if (ptr >= end) {
        	segptr->start = 0;
			break;
        }
		if (silence_count > dur) {
			//can't count trailing silence if its too long
			voice_count -= silence_count;
		}
		if (voice_count >= min) {
			//a valid segment
			segptr->end = ptr;
			segptr++;
            seg_count++;
		}
		else {
			ptr = segptr->start;
            ptr++;
			segptr->start = 0;
		}
	}//end for all segments
	if (config.seg.segments[0].start) {
		active_segment = config.seg.segments;
		segend = &config.seg.segments[127];
		for (segptr = config.seg.segments; segptr < segend; segptr++) {
        	if (segptr->start) {
				//transform as necessary
				transeg(hwnd, segptr);
				if (config.code.on) { /*if a coding set is available */
					codenow(hwnd, 1, segptr);
				}
				drawseg(hwnd);
			}
		}
	}
	drawseg(hwnd);
	SetCursor(hcurSave);
}
clearseg() {
	SEGMENT *segptr, *segend;

	segptr = config.seg.segments;
	segend = &config.seg.segments[127];
	while (segptr <= segend) {
		segptr->start = 0;
		segptr->end = 0;
		segptr->class1 = 0;
		segptr->class2 = 0;
		segptr->num_trans = 0;
		segptr->tran_start = 0;
		segptr++;
	}
	active_segment = 0;
}





		

