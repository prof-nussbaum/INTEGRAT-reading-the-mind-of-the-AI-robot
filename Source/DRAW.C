#include "FUNCTIONS.H"
#include <math.h>

//current configuration
extern CONFIG_STRUCT config;
//scale for all diagrams (target specific, not part of config)
extern int drawing_scale;
//Current segment being edited by user
extern SEGMENT *active_segment;
//font and color of ID statements
extern COLORREF crColor;
extern HFONT hfFont;
extern BOOL tfFontLoaded;
extern HINSTANCE hInst;

extern char *max_pos;
static float tran_scale = 1;
static double avg_error = 0;
static char flagdrawnot = 0;
static char flagdrawbw = 0;
// float unnorm_dim(float x, int y); 
// float norm_dim(float x, int y);
// void MoveTo(HDC hdc, unsigned int x, unsigned int y);

int drawnot(HWND hWnd) {
	int button;
	if (flagdrawnot) {
		button = MessageBox (hWnd, "Turn Draw ON?", "DRAW IS OFF", MB_YESNO);
		if (button == IDYES) {
			flagdrawnot = 0;
		}
	}
	else {
		button = MessageBox (hWnd, "Turn Draw OFF?", "DRAW IS ON", MB_YESNO);
		if (button == IDYES) {
			flagdrawnot = 1;
		}
	}
}
int drawnow(HWND hWnd) {
	int oldnot = flagdrawnot;

	flagdrawnot = 0;
	SendMessage(hWnd, WM_PAINT, 0, 0);
	flagdrawnot = oldnot;

}
int drawbw(HWND hWnd) {
	int button;
	if (flagdrawbw) {
		button = MessageBox (hWnd, "Color Draw?", "DRAW IS B/W", MB_YESNO);
		if (button == IDYES) {
			flagdrawbw = 0;
		}
	}
	else {
		button = MessageBox (hWnd, "B/W Draw?", "DRAW IS COLOR", MB_YESNO);
		if (button == IDYES) {
			flagdrawbw = 1;
		}
	}
}


int drawall(HWND hwnd) {
	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	drawwav(hwnd);
	drawseg(hwnd);
	drawtran(hwnd);
	drawcode(hwnd);
}
int drawseg(HWND hwnd) {
	HDC hdc;
	HBRUSH hbr, hbrOld;
	SEGMENT *segptr, *end;
    HCURSOR hcurSave;
	int color = 0;
	float xscale, xpos1, xpos2;
	float ymin, ymax, ypos;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	if (!config.file.open.size)
		return FALSE;

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst, "CUR_DRAW"));
    /* Segment Drawing Area */
	hdc = GetDC(hwnd);
	xscale = (drawing_scale * 15.0)/ config.file.open.size;
	ymin = 2 + (drawing_scale * 2) + (drawing_scale / 2);
	ymax = ymin + drawing_scale;

	/* clear regions above and below */
	clearsegedit(hwnd);
    clearid(hwnd);

	/* Draw bounding rectangle */
	hbr = CreateSolidBrush(RGB(0, 0, 255));
	hbrOld = SelectObject(hdc, hbr);
	Rectangle(hdc, 0, (int)ymin, drawing_scale * 15, (int)ymax);
	SelectObject(hdc, hbrOld);
	DeleteObject(hbr);

    /* Draw all line segments  */
	end = &config.seg.segments[127];
	for (segptr = config.seg.segments; segptr <= end; segptr++) {
      if (segptr->start) {
		hbr = CreateSolidBrush(RGB(((color * 16) % 128) + 127,0,0));
		hbrOld = SelectObject(hdc, hbr);
		xpos1 = (segptr->start - config.file.open.data) * xscale;
		xpos2 = (segptr->end - config.file.open.data) * xscale;
		ypos = ymin + ((color * 4) % (drawing_scale - 4)) + 2;
		Rectangle(hdc, xpos1, ypos - 2, xpos2, ypos + 2);
		color += 1;
		SelectObject(hdc, hbrOld);
		DeleteObject(hbr);
		if (segptr->class1){
			drawid(segptr, (unsigned int)xpos1, (unsigned int)xpos2, hdc);
        }
	  }
	}
	if ((active_segment >= config.seg.segments) &&
		(active_segment <= &config.seg.segments[127])) {
    	//draw active segment marker wit red box
		hbr = CreateSolidBrush(RGB(255,0,0));
		hbrOld = SelectObject(hdc, hbr);
		xpos1 = (active_segment->start - config.file.open.data) * xscale;
		xpos2 = (active_segment->end - config.file.open.data) * xscale;
		ypos = ymin - (drawing_scale / 2.0);
		Rectangle(hdc, xpos1, ypos, xpos2, ymin);
		SelectObject(hdc, hbrOld);
		DeleteObject(hbr);
	}
    SetCursor(hcurSave);
	ReleaseDC(hwnd, hdc);
    return (TRUE);
}
int clearsegedit(HWND hwnd){
	HDC hdc;
	HBRUSH hbr, hbrOld;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	hdc = GetDC(hwnd);
	/* Draw bounding rectangle */
	hbr = CreateSolidBrush(RGB(255, 255, 255));
	hbrOld = SelectObject(hdc, hbr);
	Rectangle(hdc, 0, (int)(2 + (drawing_scale * 2.0)),
		drawing_scale * 15, (int)(2 + (drawing_scale * 2.0) + (drawing_scale / 2.0)));
	SelectObject(hdc, hbrOld);
	DeleteObject(hbr);
    ReleaseDC(hwnd, hdc);
}
int clearid(HWND hwnd){
	HDC hdc;
	HBRUSH hbr, hbrOld;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	hdc = GetDC(hwnd);
	/* Draw bounding rectangle */
	hbr = CreateSolidBrush(RGB(255, 255, 255));
	hbrOld = SelectObject(hdc, hbr);
	Rectangle(hdc, 0,
		(int)(2 + (drawing_scale *(2 + .5 + 1))),
		drawing_scale * 15,                  
		(int)(2 + (drawing_scale * (2 + .5 + 1 + .5))));
	SelectObject(hdc, hbrOld);
	DeleteObject(hbr);
    ReleaseDC(hwnd, hdc);
}
int insegwin(unsigned int x, unsigned int y){
	if (!config.file.open.size) return FALSE;
	if ( (y >= 2 + (drawing_scale * 2.0) + (drawing_scale / 2.0)) &&
		 (y <= 2 + (drawing_scale * 2.0) + (drawing_scale / 2.0) +
		  		(drawing_scale)) &&
		 (x >= 0) && (x <= (drawing_scale * 15)) )
		return TRUE;
	else
		return FALSE;
}
char *getwavptr(unsigned int x){
	float xscale = (drawing_scale * 15.0)/ config.file.open.size;

	if (!config.file.open.size) return FALSE;
	return (config.file.open.data + (unsigned long)(x / xscale)); 
}

inwavwin(unsigned int x, unsigned int y){
	if (!config.file.open.size) return FALSE;
	if ( (y >= 2) &&
		 (y <= (drawing_scale * 2) + 2) &&
		 (x >= 0) && (x <= (drawing_scale * 15)) )
		return TRUE;
	else
		return FALSE;
}

drawsegedit(HWND hwnd, unsigned int x) {
	HDC hdc;
	HPEN hpen, hpenOld;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	hdc = GetDC(hwnd);
	hpen = CreatePen(PS_SOLID, 0, RGB(0,0,255));
	hpenOld = SelectObject(hdc, hpen);
		MoveToEx(hdc, x, 2 + (drawing_scale * 2),0);
		LineTo(hdc, x, 2 + (drawing_scale / 2.0) +
				(drawing_scale * 2));
		MoveToEx(hdc, x, 2 + (drawing_scale / 4.0) +
				(drawing_scale * 2),0);
		LineTo(hdc, x + (drawing_scale / 2.0),
				2 + (drawing_scale / 4.0) +
				(drawing_scale * 2));
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
    ReleaseDC(hwnd, hdc);
}
drawwav(HWND hwnd){
	RECT region;
	float xscale, amplitude, max_ampl, ypos, xpos;
    int ixpos, last_ixpos;
	unsigned long counter, ycenter;
	char *cptr, *end;
	HDC hdc;  
	HPEN hpen, hpenOld; //selected and original pen
	HBRUSH hbr, hbrOld; //original brush shade
	HCURSOR hcurSave;   // original cursor shape
	HFONT fTemp;    // Placeholder for the original font
	RECT rTemp;     // Client area needed by DrawText()
	char name[500];
    char *p1, *p2;
	int height;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	if (!config.file.open.size)
		return FALSE;
	last_ixpos = 0;
	ixpos = 0;
	max_ampl = 0;
	xscale = (drawing_scale * 15.0)/ config.file.open.size;
	if (!config.file.open.data)
		return FALSE;
	//Let user know this is a time consuming process
    hcurSave = SetCursor(LoadCursor(hInst, "CUR_DRAW"));
	hdc = GetDC(hwnd);
	ycenter = (drawing_scale * 1.0) + 2;
	end = config.file.open.data + config.file.open.size;
	/* Draw Bounding Rectangle */
	hpen = CreatePen(PS_SOLID, 0, RGB(125,125,125));
	hpenOld = SelectObject(hdc, hpen);
	if (flagdrawbw)
		hbr = CreateSolidBrush(RGB(255, 255, 255));
    else
		hbr = CreateSolidBrush(RGB(16, 255, 16));
	hbrOld = SelectObject(hdc, hbr);
	Rectangle(hdc, 0,
		(int)(ycenter - (drawing_scale * 1.0)),
		drawing_scale * 15,
		(int)(ycenter + (drawing_scale * 1.0)));
	SelectObject(hdc, hbrOld);
	DeleteObject(hbr);
	/* Draw grid 1/16 max amplitude by 1000 samples */
	for (amplitude = -128; amplitude <= 128; amplitude += 16) {
    	ypos = (amplitude / 128.0) * (drawing_scale * 1.0);
		MoveToEx(hdc, 0, (int)ypos + ycenter,0);
		LineTo(hdc, drawing_scale * 15, (int)ypos + ycenter);
	}
	for (amplitude = 0; amplitude <= config.file.open.size;
			amplitude += 1000) {
		xpos = amplitude * xscale;
		MoveToEx(hdc, (int)xpos,
			(int)(ycenter - (drawing_scale * 1.0)),0);
		LineTo(hdc, (int)xpos,
			(int)(ycenter + (drawing_scale * 1.0)));
	}
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
	/* draw maximum amplitude used for scaling */
	counter = max_pos - config.file.open.data;
	xpos = counter * xscale;
	hpen = CreatePen(PS_SOLID, 0, RGB(255,0,0));
	hpenOld = SelectObject(hdc, hpen);
	amplitude = drawing_scale * 1.0;
	MoveToEx(hdc, xpos, (int)(ycenter - amplitude),0);
	LineTo(hdc, xpos, (int)(ycenter + amplitude));
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
	/* Draw Voice Data*/
	hpen = CreatePen(PS_SOLID, 0, RGB(0,0,0));
	hpenOld = SelectObject(hdc, hpen);
	for (counter = 0, cptr = config.file.open.data;
			cptr < end; cptr++, counter++) {
		amplitude = (*cptr/128.0) * (drawing_scale * 1.0);
		xpos = counter * xscale;
		last_ixpos = ixpos;
		ixpos = (int)xpos;
        if (last_ixpos != ixpos) {
			MoveToEx(hdc, ixpos, (int)(ycenter - max_ampl),0);
			LineTo(hdc, ixpos, (int)(ycenter + max_ampl));
			max_ampl = 0;
		}
		else {
			if (amplitude > max_ampl) {
				max_ampl = amplitude;
			}
			else if (-amplitude > max_ampl) {
            	max_ampl = -amplitude;
			}            	
        }
	}
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);

	/* Draw Key */
	hpen = CreatePen(PS_SOLID, 0, RGB(255,0,0));
	hpenOld = SelectObject(hdc, hpen);
	if( tfFontLoaded == TRUE )
		fTemp = (HFONT)SelectObject( hdc, hfFont );

	ypos = ((16 / 128.0) * (drawing_scale * 1.0)) + 1;
	xpos = 1000 * xscale;
	MoveToEx(hdc, xpos, ypos,0);
	LineTo(hdc, xpos + (1000 * xscale), ypos);
	ypos += 2;
	MoveToEx(hdc, xpos, ypos,0);
	LineTo(hdc, xpos += (1000 * xscale), ypos);

	xpos += 1;
	MoveToEx(hdc, xpos, ypos,0);
	LineTo(hdc, xpos, ypos + (16 / 128.0) * (drawing_scale * 1.0));
	xpos += 1;
	MoveToEx(hdc, xpos, ypos,0);
	LineTo(hdc, xpos, ypos + (16 / 128.0) * (drawing_scale * 1.0));

	rTemp.left = xpos + (1000 * xscale);
	rTemp.right = xpos + drawing_scale * 15;
	rTemp.top = ypos;
	rTemp.bottom = drawing_scale;

	sprintf(name,"1000 bytes X 16 ticks");
	DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER | DT_CALCRECT );
    height = DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER);

	rTemp.left = rTemp.right;
	rTemp.right = xpos + drawing_scale * 15;
	rTemp.top = ypos;
	rTemp.bottom = drawing_scale;

	p1 = config.file.open.lpszFilename;
    p2 = 0;
	while (*p1++) if (*p1 == '\\') p2 = p1;
	if (p2) p1 = p2 + 1;
	else p1 = config.file.open.lpszFilename;
	sprintf(name,"  (%g bytes):%s", (float)config.file.open.size, p1);
	DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER | DT_CALCRECT );
    height = DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER);

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);
	// restore original font
	if( tfFontLoaded == TRUE )
		SelectObject( hdc, fTemp );

    /* all done */
	SetCursor(hcurSave);
    ReleaseDC(hwnd, hdc);
	return TRUE;
}
drawid(SEGMENT *segptr, unsigned int xpos1, unsigned int xpos2, HDC hdc)
{
	HFONT fTemp;    // Placeholder for the original font
	RECT rTemp;     // Client are needed by DrawText()
	char name[10];	//ID label
    int height;

	SetTextColor( hdc, RGB(0,0,0) );
	SetTextColor( hdc, crColor );
    if( tfFontLoaded == TRUE )
		fTemp = (HFONT)SelectObject( hdc, hfFont );
	rTemp.left = xpos1;
	rTemp.right = xpos2;
	rTemp.top = 2 + (drawing_scale * (2 + .5 + 1));
	rTemp.bottom = 2 + (drawing_scale * (2 + .5 + 1 + .5));

	if (config.id.ids[segptr->class1].name[0])
		sprintf(name,"%s", config.id.ids[segptr->class1].name);
	else
		sprintf(name,"%d", segptr->class1);
	DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER | DT_CALCRECT );
    height = DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER);
	rTemp.top += height;
    rTemp.bottom += height;
	if (segptr->class2) {
		if (config.id.ids[segptr->class2].name[0])
			sprintf(name,"%s", config.id.ids[segptr->class2].name);
		else
			sprintf(name,"%d", segptr->class2);
		DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER | DT_CALCRECT );
		height = DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER);
	}
	// restore original font
	if( tfFontLoaded == TRUE )
		SelectObject( hdc, fTemp );

}
int drawtran(HWND hwnd) {
	HDC hdc;
	HPEN hpen, hpenOld;
	HBRUSH hbr, hbrOld;
	SEGMENT *segptr, *end;
    HCURSOR hcurSave;
	int color = 0;
	float xscale, xpos1, xpos2;
	float ymin, ymax, ypos;
    float b_to_vect;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	if (!config.file.open.size)
		return FALSE;

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst, "CUR_DRAW"));
	/* Transform Drawing Area */
	hdc = GetDC(hwnd);
	xscale = (drawing_scale * 15.0)/ config.file.open.size;
	ymin = 2 + (drawing_scale * (2 + .5 + 1 + .5));
	ymax = ymin + (drawing_scale * 2);
    b_to_vect = (float)config.tran.interval;

	/* Draw bounding rectangle */
	hbr = CreateSolidBrush(RGB(0, 0, 0));
	hbrOld = SelectObject(hdc, hbr);
	Rectangle(hdc, 0, (int)ymin, drawing_scale * 15, (int)ymax);
	SelectObject(hdc, hbrOld);
	DeleteObject(hbr);
	ReleaseDC(hwnd, hdc);

    /* Draw for all line segments  */
	end = &config.seg.segments[127];
	for (segptr = config.seg.segments; segptr <= end; segptr++) {
	  if (segptr->start) {
      	drawonetran(hwnd, segptr);
	  }
	}
	SetCursor(hcurSave);
}
drawonetran(HWND hwnd, SEGMENT *segptr) {
	HDC hdc;
	HBRUSH hbr, hbrOld;
    HPEN hpen, hpenOld;
	float xscale, xpos1, xpos2;
	float ymin, ymax, ypos1, ypos2;
    unsigned int ixp;
	float b_to_vect;
	float dim_height;
	int trans, dim;
	float *value, vcopy;
    unsigned char color;
	HCURSOR hcurSave;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	if ((!segptr->tran_start) || (!segptr->start) ||
		(!segptr->num_trans))
		return FALSE;
	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst, "CUR_DRAW"));
	/* Transform Drawing Area */
	hdc = GetDC(hwnd);
	xscale = (drawing_scale * 15.0)/ config.file.open.size;
	ymin = 2 + (drawing_scale * (2 + .5 + 1 + .5));
	ymax = ymin + (drawing_scale * 2);
	b_to_vect = (float)config.tran.interval * xscale;
    dim_height = (drawing_scale * 2.0) / config.tran.dim;

	xpos1 = (segptr->start - config.file.open.data) * xscale;
	xpos2 = xpos1 + b_to_vect;
	value = segptr->tran_start;
	for (trans = 0; trans < segptr->num_trans; trans++) {
		ypos1 = ymin;
		ypos2 = ypos1 + dim_height;
		for (dim = 0; dim < config.tran.dim; dim++) {
			vcopy = *value;
			if ((vcopy >= 0) && (vcopy <= tran_scale)) {
				color = (vcopy / tran_scale) * 239;
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(color, color, color));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(color, 0, 0));
				hpenOld = SelectObject(hdc, hpen);
			}
			else if ((-vcopy >= 0) && (-vcopy <= tran_scale)) {
				color = (-vcopy / tran_scale) * 239;
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(color, color, color));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, color));
				hpenOld = SelectObject(hdc, hpen);
			}
			else if (vcopy >= 0) {
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
				hpenOld = SelectObject(hdc, hpen);
			}
			else {
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
				hpenOld = SelectObject(hdc, hpen);
			}
			for (ixp = xpos1; ixp < xpos2; ixp++) {
				MoveToEx(hdc, ixp, ypos1,0);
				LineTo(hdc, ixp, ypos2);
            }
			SelectObject(hdc, hpenOld);
			DeleteObject(hpen);

			ypos1 += dim_height;
			ypos2 += dim_height;
			value++;
		}
		xpos1 += b_to_vect;
		xpos2 += b_to_vect;
	}
	ReleaseDC(hwnd, hdc);
	SetCursor(hcurSave);
}
drawcode(HWND hwnd) {
	HDC hdc;
	HBRUSH hbr, hbrOld;
    HPEN hpen, hpenOld;
	float xscale, xpos1, xpos2;
	float ymin, ymax, ypos1, ypos2;
    unsigned int ixp;
	float dim_height, vect_width, vcopy;
	int trans, dim;
	float *value;
	unsigned char color;
	unsigned int num_units, num_dim;
	float *units;
	float *stats;
	float max;
    unsigned long x;
	HCURSOR hcurSave;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	if (!(config.code.on && config.file.cod.size))
		return FALSE;

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst, "CUR_DRAW"));

	/* coding unit and statistical data */
	units = (float *)config.file.cod.data;
	if (!units) return FALSE;
	num_units = config.id.num;
	if (!num_units) num_units = 127;
	num_dim = config.tran.dim;
	if (!num_dim) return FALSE;
    /* get to end of units */
	stats = units + (num_dim * num_units);

	/* Codes Drawing Area */
	hdc = GetDC(hwnd);
	ymin = 2 + (drawing_scale * (2 + .5 + 1 + .5 + 2 + .5));
	ymax = ymin + (drawing_scale * 2);
    vect_width = (drawing_scale * 15.0) / num_units; 
	dim_height = (drawing_scale * 2.0) / config.tran.dim;

	xpos1 = 0;
	xpos2 = xpos1 + vect_width;
	value = units;
	for (trans = 0; trans < num_units; trans++) {
		ypos1 = ymin;
		ypos2 = ypos1 + dim_height;
		for (dim = 0; dim < num_dim; dim++) {
			vcopy = unnorm_dim(*value, dim);
			if ((vcopy >= 0) && (vcopy <= tran_scale)) {
				color = (vcopy / tran_scale) * 239;
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(color, color, color));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(color, 0, 0));
				hpenOld = SelectObject(hdc, hpen);
			}
			else if ((-vcopy >= 0) && (-vcopy <= tran_scale)) {
				color = (-vcopy / tran_scale) * 239;
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(color, color, color));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, color));
				hpenOld = SelectObject(hdc, hpen);
			}
			else if (vcopy >= 0) {
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
				hpenOld = SelectObject(hdc, hpen);
			}
			else {
				if (flagdrawbw)
					hpen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
                else
					hpen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
				hpenOld = SelectObject(hdc, hpen);
			}
			for (ixp = xpos1; ixp < xpos2; ixp++) {
				MoveToEx(hdc, ixp, ypos1,0);
				LineTo(hdc, ixp, ypos2);
            }
			SelectObject(hdc, hpenOld);
			DeleteObject(hpen);

			ypos1 += dim_height;
			ypos2 += dim_height;
			value++;
		}
		xpos1 += vect_width;
		xpos2 += vect_width;
	}

    /* now draw statistics for each unit */

	drawcodestat(hwnd);

	value += num_dim;/* get to the end of dummy */
	value += num_units;/* get to the end of stats */

	/* now draw random vector selection statistics */
	/* Rand Stats Drawing Area */
	ymin = 2 + (drawing_scale * (2 + .5 + 1 + .5 + 2 + .5 + 2 + .25 + .5 + .25));
	ymax = ymin + (drawing_scale * 0.5);
	stats = value;
	vect_width = (drawing_scale * 15.0) / config.id.count;
	max = 0;
	for (x = 0; x < config.id.count; x++) {
		if (*value > max) max = *value;
		value++;
	}
	value = stats;
	xpos1 = 0;
	xpos2 = xpos1 + vect_width;
	ypos1 = ymin;
    ypos2 = ymax;
	if (max) max = 255 / max;
	for (x = 0; x < config.id.count; x++) {
		color = *value * max;
		if (flagdrawbw)
			hpen = CreatePen(PS_SOLID, 0, RGB(color, color, color));
        else
			hpen = CreatePen(PS_SOLID, 0, RGB(0, color, 0));
		hpenOld = SelectObject(hdc, hpen);
		for (ixp = xpos1; ixp < xpos2; ixp++) {
			MoveToEx(hdc, ixp, ypos1,0);
			LineTo(hdc, ixp, ypos2);
        }
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
		xpos1 += vect_width;
		xpos2 += vect_width;
		value++;
    }

	ReleaseDC(hwnd, hdc);
	SetCursor(hcurSave);
}
drawcodestat(HWND hwnd) {
	HBRUSH hbr, hbrOld;
    HPEN hpen, hpenOld;
	float xscale, xpos1, xpos2;
	float ymin, ymax, ypos1, ypos2;
    unsigned int ixp;
	float dim_height, vect_width;
	int trans, dim;
	float *value;
	unsigned char color;
	unsigned int num_units, num_dim;
	float *units;
	float *stats;
	float max;
    unsigned long x;
	HDC hdc;
	HCURSOR hcurSave;

	if (flagdrawnot) {
		shownot(hwnd);
		return(FALSE);
    }
	if (!(config.code.on && config.file.cod.size))
		return FALSE;

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst, "CUR_DRAW"));

	/* coding unit and statistical data */
	units = (float *)config.file.cod.data;
	if (!units) return FALSE;
	num_units = config.id.num;
	if (!num_units) num_units = 127;
	num_dim = config.tran.dim;
	if (!num_dim) return FALSE;
    /* get to end of units */
	stats = units + (num_dim * num_units);
	/* get to end of dummy */
	stats += num_dim;

	/* Codes Drawing Area */
	hdc = GetDC(hwnd);

	/* now draw statistics for each unit */
	/* Stats Drawing Area */
	vect_width = (drawing_scale * 15.0) / num_units;
	ymin = 2 + (drawing_scale * (2 + .5 + 1 + .5 + 2 + .5 + 2 + .25));
	ymax = ymin + (drawing_scale * 0.5);
	value = stats;
    max = 0;
	for (trans = 0; trans < num_dim; trans++) {
		if (*value > max) max = *value;
		value ++;
	}
	value = stats;
	xpos1 = 0;
	xpos2 = xpos1 + vect_width;
	ypos1 = ymin;
    ypos2 = ymax;
	if (max) max = 255 / max;
	for (trans = 0; trans < num_units; trans++) {
		color = *value * max;
		if (flagdrawbw)
			hpen = CreatePen(PS_SOLID, 0, RGB(color, color, color));
        else
			hpen = CreatePen(PS_SOLID, 0, RGB(0, color, 0));
		hpenOld = SelectObject(hdc, hpen);
		for (ixp = xpos1; ixp < xpos2; ixp++) {
			MoveToEx(hdc, ixp, ypos1,0);
			LineTo(hdc, ixp, ypos2);
		}
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
		xpos1 += vect_width;
		xpos2 += vect_width;
		value++;
    }

	ReleaseDC(hwnd, hdc);
	SetCursor(hcurSave);
}
shownot(HWND hwnd) {
	HDC hdc;
	HCURSOR hcurSave;
	HFONT fTemp;    // Placeholder for the original font
	RECT rTemp;     // Client are needed by DrawText()
	char name[80];	//ID label
    int height;

	//Let the user know this is a time consuming process
	hcurSave = SetCursor(LoadCursor(hInst, "CUR_DRAW"));

	/* Codes Drawing Area */
	hdc = GetDC(hwnd);

	SetTextColor( hdc, RGB(255,0,0) );
	SetTextColor( hdc, crColor );
	if( tfFontLoaded == TRUE )
		fTemp = (HFONT)SelectObject( hdc, hfFont );
	rTemp.left = 0;
	rTemp.right = drawing_scale * 15;
	rTemp.top = 2 + (drawing_scale * (2 + .5 + 1 + .5 + 2));
	rTemp.bottom = 2 + (drawing_scale * (2 + .5 + 1 + .5 + 2 + .25));
	sprintf(name,"VISUALLIZATION SUSPENDED");
	DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER | DT_CALCRECT );
    height = DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER);
	// restore original font
	if( tfFontLoaded == TRUE )
		SelectObject( hdc, fTemp );

	ReleaseDC(hwnd, hdc);
	SetCursor(hcurSave);
}
drawstates(HWND hwnd, float *states, float errors)
{
	int x;
	RECT region;
	DWORD ypos, xpos;
    DWORD ymin, ymax, xmin, xmax;
	HDC hdc;
	HPEN hpen, hpenOld; //selected and original pen
	int color;

	if (flagdrawnot) {
		return(FALSE);
	}
	/* Drawing Area */
	hdc = GetDC(hwnd);
	ymin = 2;
	ymax = 2 + (drawing_scale * (2 + .5 + 1 + .5 + 2 + .5 + 2));
	xmin = 1;
    xmax = (drawing_scale * 15);
	xpos = config.code.iter % (drawing_scale * 15);
	ypos = 2 + (drawing_scale * (2 + .5 + 1 + .5 + 2 + .5 + 2));
	hpen = CreatePen(PS_SOLID, 0, RGB(0,0,0));
	hpenOld = SelectObject(hdc, hpen);

	/* clear state area */
	if ((ypos < ymin) || (ypos > ymax)) ypos = 1;
	if ((xpos < xmin) || (xpos > xmax)) xpos = 1;
	MoveToEx(hdc,xpos , ypos,0);
	LineTo(hdc,xpos , ypos - (drawing_scale * 2));
	/* clear error area */
	ypos = 2 + (drawing_scale * 2);
	if ((ypos < ymin) || (ypos > ymax)) ypos = 1;
	if ((xpos < xmin) || (xpos > xmax)) xpos = 1;
	MoveToEx(hdc,xpos , ypos,0);
	LineTo(hdc,xpos , ypos - (drawing_scale * 2));

	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);

	for (x = 0; x < config.id.num; x++){
		color = ((128.0 / (config.id.num - 1)) * x) + 127;
		hpen = CreatePen(PS_SOLID, 0, RGB(color,color ,color));
		hpenOld = SelectObject(hdc, hpen);
		if (states[x] < 0.0) states[x] = 0.0;
        if (states[x] > 1.0) states[x] = 1.0;
		ypos = 2 + (DWORD)((1.0 - states[x]) * drawing_scale * 2) +
			(drawing_scale * (2 + .5 + 1 + .5 + 2 + .5));
		MoveToEx(hdc, xpos , ypos,0);
		LineTo(hdc, xpos + 1, ypos);
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
	}
	if (!(config.code.iter % 10)) {
		avg_error += errors;
		avg_error /= 10.0;
		if (avg_error < 0.0) avg_error = 0.0;
		if (avg_error > 1.0) avg_error = 1.0;
		ypos = 2 + ((1.0 - avg_error) * drawing_scale * 2);
		hpen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
		hpenOld = SelectObject(hdc, hpen);
		MoveToEx(hdc, xpos, ypos,0);
		LineTo(hdc, xpos - 10, ypos);
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
		avg_error = 0.0;
	}
	else {
		avg_error += errors;
	}
	if (errors < 0.0) errors = 0.0;
	if (errors > 1.0) errors = 1.0;
	ypos = 2 + ((1.0 - errors) * drawing_scale * 2);
	hpen = CreatePen(PS_SOLID, 0, RGB(255, 255, 0));
	hpenOld = SelectObject(hdc, hpen);
	MoveToEx(hdc, xpos, ypos,0);
	LineTo(hdc, xpos + 1, ypos);
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);

	ReleaseDC(hwnd, hdc);
}
