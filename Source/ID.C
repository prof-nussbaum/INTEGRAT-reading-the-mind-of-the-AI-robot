#include "FUNCTIONS.H"
#define HALFMEG (unsigned long)(1024.0 * 1024.0 * 0.5)

void cmidbox(int *num, char *name);
void cmid2box(int *num);
extern CONFIG_STRUCT config;
extern SEGMENT *active_segment;

idstep(HWND hWnd) {

	int number, button;
	char name[10];
    SEGMENT *segptr, *end;


	if ((active_segment >= config.seg.segments) &&
		(active_segment <= (end = &config.seg.segments[127])))
	{
		playseg(hWnd, active_segment);
		segptr = active_segment;
	    segptr++;
		while (segptr <= end) {
			if (segptr->start) break;
			if (segptr == end) segptr = config.seg.segments - 1;
			if (segptr == active_segment) break;
	        segptr++;            
		}
    }
	active_segment = segptr;
	drawseg(hWnd);
}
idnext(HWND hWnd) {

	int number, button;
	char name[10];
    SEGMENT *segptr, *end;

	if ((active_segment >= config.seg.segments) &&
		(active_segment <= (end = &config.seg.segments[127])))
	{
		if (!active_segment->start){
        	//something is wrong;
			MessageBox(hWnd,"Active Segment Undefined!","SOMETHING IS WRONG",MB_OK|MB_ICONSTOP);
			return FALSE;
        }
		number = active_segment->class1;
        if (number) {
			strcpy(name, config.id.ids[number].name);
        }
		else {
			name[0] = 0;
        }
		playseg(hWnd, active_segment);
		cmidbox(&number, name);
		if (number > config.id.num) {
			MessageBox(hWnd,"ID Number too large","ID NO GOOD",MB_OK|MB_ICONSTOP);
			return FALSE;
		}
		if ((number) && (number == active_segment->class1)){
        	//User likes the name (changed or not)
			strcpy(config.id.ids[number].name, name);
		}
		if ((number) && (number != active_segment->class1)){
			//Does user whant a new name for the new number??
			if ((strcmp(config.id.ids[number].name, name)) &&
				(name[0])) {
				strcpy(config.id.ids[number].name, name);
            }
		}
		active_segment->class1 = number;
		if (config.id.pair) {
			button = MessageBox (hWnd, "Is there a second ID", "ID PAIR", MB_YESNO);
			if (button == IDYES) {
				number = active_segment->class2;
			    if (number) {
					strcpy(name, config.id.ids[number].name);
			    }
				else {
					name[0] = 0;
			    }
				cmidbox(&number, name);
				if (number > config.id.num) {
					MessageBox(hWnd,"ID Number too large","ID NO GOOD",MB_OK|MB_ICONSTOP);
					return FALSE;
				}
				if ((number) && (number == active_segment->class2)){
			       	//User likes the name (changed or not)
					strcpy(config.id.ids[number].name, name);
				}
				if ((number) && (number != active_segment->class2)){
					//Does user whant a new name for the new number??
					if ((strcmp(config.id.ids[number].name, name)) &&
						(name[0])) {
						strcpy(config.id.ids[number].name, name);
					}
				}
				active_segment->class2 = number;
        	}
        }
		segptr = active_segment;
        segptr++;
		while (segptr <= end) {
			if (segptr->start) break;
			if (segptr == end) segptr = config.seg.segments - 1;
			if (segptr == active_segment) break;
            segptr++;            
		}
		active_segment = segptr;
	}
    drawseg(hWnd);
}
idnum(HWND hWnd) {cmid2box(&config.id.num); }
idpair(HWND hWnd) {
	int button;

	if (config.id.pair) {
		button = MessageBox (hWnd, "Turn Pairing Off?", "PAIRING IS ON", MB_YESNO);
		if (button == IDYES) {
			config.id.pair = 0;
		}
	}
	else {
		button = MessageBox (hWnd, "Turn Pairing On?", "PAIRING IS OFF", MB_YESNO);
		if (button == IDYES) {
			config.id.pair = 1;
		}
	}
}
idfile(HWND hWnd) {
	SEGMENT *segptr, *end;
	float *hfloat, *hdim;
	unsigned char *hchar;
    int num_trans, num_dim;

	if (!config.file.tra.data) {
//		if (!(config.file.tra.data = farcalloc(1, HALFMEG))) {
		if (!(config.file.tra.data = (char *) calloc(1, HALFMEG))) {
			MessageBox(hWnd,"Can't Alloc 1/2 MEG!","Voice File Results",MB_OK);
			return(0);
		}
		else {
			config.file.tra.cur_pos = config.file.tra.data;
            config.file.tra.size = 0;
		}
	}
	end = &config.seg.segments[127];
	hchar = config.file.tra.cur_pos;
	for (segptr = config.seg.segments; segptr <= end; segptr++) {
		if ((segptr->tran_start) && (segptr->num_trans)) {
        	*(hchar++) = segptr->class1;
			*(hchar++) = segptr->class2;
			*(hchar++) = 'C';
			*(hchar++) = 'K';
			hfloat = (float *)hchar;
			*(hfloat++) = (float)segptr->num_trans;
			num_trans = segptr->num_trans;
            hdim = segptr->tran_start;
            while (num_trans--) {
				num_dim = config.tran.dim;
				while (num_dim--) {
					*(hfloat++) = *(hdim++);
				}
			}
			if (segptr->class1) {
				config.id.ids[segptr->class1].examples +=
					segptr->num_trans;
            }
			if (segptr->class2) {
				config.id.ids[segptr->class2].examples +=
					segptr->num_trans;
			}
			config.id.count += segptr->num_trans;
			hchar = (char *)hfloat;
		}/* end if valid segment */
	}/* end for all segments */
//	config.file.tra.cur_pos = hfloat;
	config.file.tra.cur_pos = (char *)hfloat;
	config.file.tra.size = config.file.tra.cur_pos
						- config.file.tra.data;
	/* show user results of saving transform data */
	cmconbox();
}


