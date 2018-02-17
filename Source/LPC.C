/* autocorrelation method LPC coefficients */
#include "FUNCTIONS.H"

#include <math.h>
#include <float.h>


/* insure valid LPC dimensions */
static void validate_dim();
/* allocate/deallocate workspace RAM */
static int getram();
static void freeram();
/* LPC of voice data segment */
static void start_lpc();
static int get_frame();
static void startunlpc();
static void ucalc(void);
/* lpc of one window */
// void tsolve(void);
/* reconstruct voice from LPC coefficients */
//	char *lpcuntran(float *tran, int num_trans,
//			char *voice, TRAN_STRUCT *dims);
/* perform LPC coefficient calcualtions */
//	float *lpctran(char *start,
//				char *end,
//				float *transform,
//				int *num_trans,
//                TRAN_STRUCT *dims);


#define	MAX_INTERVAL 2000
#define	MAX_WINDOW	 2000
#define	MAX_COEFF		 128

/* working pointers */
static char *sstart, *s_end;
static float *stransform;
static int *snum_trans;
static TRAN_STRUCT *sdims;
static float	pi = 3.14159265;
static int window, interval, coeff;
static float power, error;

/* allocated data */
static char  far *input;
static char  far *output;
static float far *work;
static float far *pred_coeff;
static float far *reflec_coeff;
static float far *filt;
static float far *autocorr;
static HGLOBAL hinput, houtput, hwork, hpred_coeff,
				hreflec_coeff, hfilt, hautocorr;
static int one_byte = 1;

float *lpctran(char *start,
				char *end,
				float *transform,
				int *num_trans,
				TRAN_STRUCT *dims) {
	char allocate = TRUE;
	
	sstart = start;
	s_end = end;
	stransform = transform;
	snum_trans = num_trans;
	sdims = dims;

	if ((!sstart) || (!s_end) || (!stransform) ||
		(!snum_trans) || (!sdims))
		return FALSE;
	validate_dim();	/* validate dimensions */
	allocate = getram();
	if (allocate) {
		*snum_trans = 0;
		start_lpc();
	}
	freeram();
	if (!allocate)
		return FALSE;
	else
		return (stransform);
}
static void validate_dim(){
	window = sdims->size;
	interval = sdims->interval;
	coeff = sdims->dim - 2;/* always have power and error also*/
	if (coeff <= 0)
		coeff = 1;
	if (coeff > MAX_COEFF)
    	coeff = MAX_COEFF;
	if (window < coeff )
		window = coeff ;
	if (window > MAX_WINDOW)
		window = MAX_WINDOW;
	if (interval > window)
		interval = window;
	if (interval <= 0)
		interval = 1;
	/* reassign corrected dimension values */
	sdims->size = window;
	sdims->interval = interval;
	sdims->dim = coeff + 2;
    /* unused for LPC */
	sdims->rms = 0;
    sdims->nopitch = 0;
}
static int getram(){
	int allocate = TRUE;
	DWORD iosize = sizeof(char) * (window + 1);
	DWORD worksize = sizeof(float) * (window + 1);
	DWORD coeffsize = sizeof(float) * (coeff + 1);

	/* allocate workspace */
	hinput = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, iosize);
	if (!(input = GlobalLock(hinput))) allocate = FALSE;  
	houtput = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, iosize);
	if (!(output = GlobalLock(houtput))) allocate = FALSE;  
	hwork = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, worksize);
	if (!(work = GlobalLock(hwork))) allocate = FALSE;  
	hpred_coeff = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, worksize);
	if (!(pred_coeff = GlobalLock(hpred_coeff))) allocate = FALSE;  
	hreflec_coeff = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, worksize);
	if (!(reflec_coeff = GlobalLock(hreflec_coeff))) allocate = FALSE;  
	hfilt = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, worksize);
	if (!(filt = GlobalLock(hfilt))) allocate = FALSE;  
	hautocorr = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, coeffsize);
	if (!(autocorr = GlobalLock(hautocorr))) allocate = FALSE;  
	return (allocate);
}
static void freeram() {
	/* deallocate resources*/
	GlobalUnlock(hinput);
    GlobalFree(hinput);
	GlobalUnlock(houtput);
    GlobalFree(houtput);
	GlobalUnlock(hwork);
    GlobalFree(hwork);
	GlobalUnlock(hpred_coeff);
    GlobalFree(hpred_coeff);
	GlobalUnlock(hreflec_coeff);
    GlobalFree(hreflec_coeff);
	GlobalUnlock(hfilt);
    GlobalFree(hfilt);
	GlobalUnlock(hautocorr);
    GlobalFree(hautocorr);
}
char *lpcuntran(float *tran, int num_trans,
			char *voice, TRAN_STRUCT *dims){
	char allocate = TRUE;

	sstart = voice;
	s_end = voice;
	stransform = tran;
	snum_trans = &num_trans;
	sdims = dims;
	if ((!sstart) || (!s_end) || (!stransform) || (!sdims))
		return FALSE;

    validate_dim();
    allocate = getram();
	if (allocate) {
		startunlpc();
	}
    freeram();
	if (!allocate)
		return FALSE;
	else
		return (s_end);
}
/* solve for one frame of size BWIND */
void tsolve()
{
	int i, j, k;
	float sum, reflec, predi, predj, max, scale;
    float ai, aj;//temp pred coeff

	/* remove DC component */
	sum = 0;
	for (i = 1; i <= window; i++) {
		sum += work[i];
	}
    sum /= window;
	for (i = 1; i <= window; i++) {
		work[i] -= sum;
    }
	/* compute autocorrelations*/
	for (i = 0; i <= coeff; i++) {
		sum = 0;
		for (k = 1; k <= (window - coeff); k++) {
			sum += work[k] * work[k + i];
		}
		autocorr[i] = sum;
		pred_coeff[i] = 0;
	}
	power = autocorr[0];
	if (power) {
		/* compute reflector and predictor coefficients */
		error = power;
		pred_coeff[0] = 1;
		for (k = 1; k <= coeff; k++) {
			/* new reflec coeff */
			sum = 0;
			for (i = 1; i <= k; i++) {
				sum += pred_coeff[k - i] * autocorr[i];
            }
			reflec = -sum / error;
			/* reflec should always be in unit circle */
            /* Made 1 / reflec */
			//if (reflec >= 1) reflec = 1.0 / reflec;
			//else if (-reflec >= 1) reflec = 1.0 / reflec;
			/* new pred coeff */
			pred_coeff[k] = reflec;
			for (i = 1; i <= k/2; i++) {
				ai = pred_coeff[i];
				aj = pred_coeff[k - i];
				pred_coeff[i] = ai + reflec * aj;
				pred_coeff[k - i] = aj + reflec * ai;
			}
#if 0
			//duh
			for (i = 1; i < k; i++) {
				reflec_coeff[i] = pred_coeff[i];
			}
			for (i = 1; i <= k - 1; i++) {
				pred_coeff[i] = reflec_coeff[i] +
					(reflec * reflec_coeff[k - i]);
			}
#endif
			/* new error */
			error = error * (1.0 - (reflec * reflec));
			if (error <= 0.00001) break;
		}
	}/* end if power > 0 */
	/* removed stability check */
#if 0
	/* Insure stability by scaling to inside unit circle*/

	max = 0;
	for (i = 1; i <= coeff; i++) {
		if (pred_coeff[i] > max) max = pred_coeff[i];
		else if (-pred_coeff[i] > max) max = -pred_coeff[i];
	}
	if (max > 0.9) scale = 0.9 / max;
	else scale = 1;
	for (i = 1; i <= coeff; i++) {
		pred_coeff[i] *= scale;
	}
	/* clip bad coefficients */
	for (i = 1; i <= coeff; i++) {
		if(pred_coeff[i] > 1.0)
			pred_coeff[i] = 1.0;
		else if (pred_coeff[i] < -1.0)
        	pred_coeff[i] = -1.0;
	}
#endif
	if ((!power)/* || (error <= 0)*/) {
		/* these pred coeff are no good */
		for (i = 1; i <= coeff; i++) {
			pred_coeff[i] = reflec_coeff[i] = 0;
		}
	}
}

static void start_lpc()
{
	int inc, i, j, k, step, chunks, sqcnt;
	float	val, prev, cur, cur1, cur2, cur3;
	float	maxmag, *save, sumsq;

	/* make hamming window array */
	val = 2.0*pi/((float)window);
	for(i=1,cur = 0.0; i <= window; i++, cur += val) {
		if (sdims->hamm)
		/* Hamming 		*/
			filt[i] = (-(0.84 * cos(cur)) + 1) / 2;
		/* Hanning
			filt[i] = (1 - cos(cur)) / 128.0;*/
		else
			filt[i] = 1;
		input[i] = output[i] = work[i] = 0.0;
    }
	/* initialize things */
	for(i=0; i <= coeff; i++){
		pred_coeff[i] = reflec_coeff[i] = autocorr[i] = 0;    	
	}
	while (get_frame()) {
		tsolve();
		*snum_trans += 1;
		*(stransform++) = power;
		for (i = 1; i <= coeff; i++) {
			*(stransform++) = pred_coeff[i];
		}
        *(stransform++) = error;
	}
}
static int get_frame(){
	int i;

	/* shift over input */
	for (i = 1; i <= (window - interval); i++) {
		input[i] = input[i + interval];
	}
	for (i = window - interval + 1; i <= window; i++) {
		input[i] = *(sstart++);
		if (sstart > s_end) { return FALSE; }
	}
    /* hamming window */
	for (i = 1; i <= window; i++) {
		work[i] = input[i] * filt[i];
	}
    return TRUE;
}
static void startunlpc() {
	int i;
    float cur, val;

	/* make hamming window array */
	val = 2.0*pi/((float)window);
	for(i=1,cur = 0.0; i <= window; i++, cur += val) {
    	if (sdims->hamm)
		/* Hamming */
			filt[i] = (-(0.84 * cos(cur)) + 1) / 2.0;
		/* Hanning 
			filt[i] = (1 - cos(cur)) / 128.0; */
		else
			filt[i] = 1;
		work[i] = 0.0;
    }
	while (*snum_trans) {
		power = *(stransform++);
		for (i = 1; i <= coeff; i++) {
			pred_coeff[i] = *(stransform++);
		}
		error = *(stransform++);
        *snum_trans -= 1;
		ucalc();
	}
}
static void ucalc(void) {
	float far *fptr, far *fptr2, far *end;
	int i,j,k;
	float sum, max, scale, this_pow;
	/* perform unlpc using fixed pitch of 200 bytes period*/

	/* fix any unstable and too small pred_coeff */
/*	for (i = 1; i < coeff; i++) {
		if (pred_coeff[i] > 1) pred_coeff[i] = 0.999;
		if (pred_coeff[i] < -1) pred_coeff[i] = -0.999;
		if ((pred_coeff[i] > -.0001) && (pred_coeff[i] < .0001))
			pred_coeff[i] = 0;
	} */
	/* use reflec_coeff array for temprary workspace */
	/* use work to store running output values */
	end = &reflec_coeff[window];
	for (fptr = &reflec_coeff[1]; fptr <= end; fptr++) {
		*fptr = 0;
	}
	for (fptr = &reflec_coeff[1], i = 1; fptr <= end;
			i++, fptr++) {
		sum = 0;
		for (j = 1; j <= coeff; j++) {
			if (error <= 0) break;
			if (j >= i) break;
			if ((*(fptr - j) > 0.00001) ||
				(-*(fptr - j) > 0.00001)){
				sum += *(fptr - j) * pred_coeff[j];
            }
		}
		if (one_byte == 1) {
			*fptr = 0.9;
		}
		else {
			*fptr = -sum;
		}
		one_byte++;
        if (one_byte == 200) one_byte = 1;
	}
	/* shift over last output frame */
	end = &work[window - interval];
	for (fptr = &work[1]; fptr <= end; fptr++) {
		*fptr = *(fptr + interval);
	}
	/* AVERAGE WITH NEW VALUE (UNHAMMING-WINDOWED) */
	fptr2 = &reflec_coeff[1];
	for (fptr = &work[1], i = 1; fptr <= end; fptr++, i++) {
		if (filt[i]) {
			*fptr = (*fptr + (*fptr2 / filt[i])) / 2.0;
		}
		fptr2++;
	}
	/* then fill in remainder */
	end = &work[window];
	for (; fptr <= end; fptr++, i++) {
		if (filt[i]) {
			*fptr = *fptr2 / filt[i];
		}
        fptr2++;
	}
	/* scale back to original RMS */
	this_pow = 0;
	max = 0;
	end = &work[window - coeff];
	for (fptr = &work[1]; fptr <= end; fptr++) {
		this_pow += *fptr * *fptr;
		if (*fptr > max) {
			max = *fptr;
        }
		else if (-*fptr > max) {
			max = -*fptr;
		}
	}
	if (this_pow) {
		scale = power / this_pow;
	}
	else {
		scale = 0;
	}
	/* see if current scale will blow away peak amplitude */
	if ((scale * max) > 126.0) {
    	/* if so, scale to peak amplitude for understandability */
		scale = 126.0 / max;
	}
	/* just scale to a flat peak amp. */
	//scale = 126.0 / max;
	end = &work[interval];
	for (fptr = &work[1]; fptr <= end; fptr++)
	{
		*(s_end++) = (char)(*fptr * scale);
	}
}
// Here's hoping Microsoft has fixed that problem in the past 20 years... Removing the matherr function
/* have to write my own math error handler because the default one
	keeps locking up the system */
	/*
//int matherr (struct exception *a)
int matherr (struct _exception *a)
{
	int x;
	x = 1;
    if (!x)
		exit(23);
	if (a->type == OVERFLOW) {//overflow
    	return(0);
    }
	if (a->type == UNDERFLOW) {//underflow
		a->retval = 0.0;
        return(1);
	}
	if (a->type == DOMAIN)// out of valid domain
	if (a->type == SING)// result is at singularity point
	if (a->type == TLOSS);//loss of significant digits
	return 0;
}*/
