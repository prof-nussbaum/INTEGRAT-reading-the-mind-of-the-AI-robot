#include <math.h>
#include "FUNCTIONS.H"

static int getram();
static void freeram();

#define PI 3.14159265359

float  far *wr, far *wi;  /* W exponents */
float  far *xr, far *xi;  /* workspace */
int    far *reverse;	 /* bit reverse converion array */
char   far *in;		 /* input */
float  far *fft_vector;
float  far *out;	 /*output*/
float  far *hamm;	 /*for raised cosine multiplier */
int    dimension, logdim; /* size and log base 2 size of FFT */
int    overlap; 	 /* number of bytes overlap */
double saved_rms; /* rms of original voice*/

int debug = 1;
static char *sstart; 		//start of voice data
static char *s_end;			//end of voice data
static float *stransform;	//place to put transofmation data
static int *snum_trans;			//number of transformation frames
static TRAN_STRUCT *sdims;		//transform dimensions
static HGLOBAL hwr, hwi, hxr, hxi, hfft_vector, hhamm, hreverse, hin, hout;
static DWORD floatsize, intsize, charsize;

char *fftuntran(float *tran, int num_trans,
		  char 	*voice, TRAN_STRUCT *dims) {

	char allocate = TRUE;

	sstart = voice;
	s_end = voice;
	stransform = tran;
	snum_trans = &num_trans;
	sdims = dims;
	if ((!sstart) || (!s_end) || (!stransform) || (!sdims))
		return FALSE;

    allocate = getram();
    if (allocate) {
		get_command();	/* get the dimension of the transform  */
	    make_w();	    /* generate the w exponentials */
	    make_reverse(); /* generate the bit reverse table */
	    make_hamm();	/* generate hamming window */
		startunfft();	    /* do untransform */
	}
    freeram();
	if (!allocate)
		return FALSE;
	else
		return (s_end);
}


float *ffttran(char *start,
					char *end,
					float *transform,
					int *num_trans,
					TRAN_STRUCT *dims)
{
	char allocate = TRUE;

	sstart = start;
	s_end = end;
	stransform = transform;
	snum_trans = num_trans;
	sdims = dims;
	if ((!sstart) || (!s_end) || (!stransform) || (!sdims))
		return FALSE;

    allocate = getram();
    if (allocate) {
	    *snum_trans = 0;
	    get_command();	/* get the dimension of the transform  */
	    make_w();	    /* generate the w exponentials */
	    make_reverse(); /* generate the bit reverse table */
	    make_hamm();	/* generate hamming window */
		startfft();	    /* do tranform/untransform */
	}
    freeram();
	if (!allocate)
		return FALSE;
	else
		return (stransform);
}

static int getram() {
	int allocate = TRUE;

	floatsize = sizeof(float) * 1024;
	intsize = sizeof(int) * 1024;
	charsize = sizeof(char) * 1024;
    /* Allocate workspace */
	hwr = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, floatsize);
    if (!(wr = GlobalLock(hwr))) allocate = FALSE;
	hwi = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, floatsize);
    if (!(wi = GlobalLock(hwi))) allocate = FALSE;
	hxr = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, floatsize);
    if (!(xr = GlobalLock(hxr))) allocate = FALSE;
	hxi = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, floatsize);
    if (!(xi = GlobalLock(hxi))) allocate = FALSE;
	hfft_vector = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, floatsize);
	if (!(fft_vector = GlobalLock(hfft_vector))) allocate = FALSE;
	hhamm = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, floatsize);
    if (!(hamm = GlobalLock(hhamm))) allocate = FALSE;
	hreverse = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, intsize);
    if (!(reverse = GlobalLock(hreverse))) allocate = FALSE;
	hin = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, charsize);
    if (!(in = GlobalLock(hin))) allocate = FALSE;
	hout = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, floatsize);
	if (!(out = GlobalLock(hout))) allocate = FALSE;

    return (allocate);
}
static void freeram() {
	/*deallocate resources */
	GlobalUnlock(hwr);
    GlobalFree(hwr);
	GlobalUnlock(hwi);
    GlobalFree(hwi);
	GlobalUnlock(hxr);
    GlobalFree(hxr);
	GlobalUnlock(hxi);
    GlobalFree(hxi);
	GlobalUnlock(hfft_vector);
	GlobalFree(hfft_vector);
	GlobalUnlock(hhamm);
    GlobalFree(hhamm);
	GlobalUnlock(hreverse);
    GlobalFree(hreverse);
	GlobalUnlock(hin);
    GlobalFree(hin);
	GlobalUnlock(hout);
	GlobalFree(hout);
}

/* get the dimension of transform */
get_command()
{
	dimension = sdims->size;
	if (dimension >= 1024) dimension = 1024;
	else if (dimension >= 512) dimension = 512;
	else if (dimension >= 256) dimension = 256;
	else if (dimension >= 128) dimension = 128;
	else if (dimension >= 64) dimension = 64;
	else dimension = 32;
	sdims->size = dimension;
	if (sdims->dim > (sdims->size ) + 1)
		sdims->dim = (sdims->size ) + 1;
	if (sdims->interval > sdims->size)
		sdims->interval = sdims->size;
	overlap = sdims->size - sdims->interval;
    if (overlap < 0) overlap = 0;
	debug = 0;
}
/* generate the complex exponential table WsubN*/
make_w()
{
    int i;
	float arg, step;

	step = 2 * PI / dimension;
    arg = 0;
    wr[0] = 1.0;
    wi[0] = 0.0;
    for(i = 1; i < dimension; i++)
    {
		arg += step;
		wr[i] = cos(arg);
		wi[i] = sin(arg);
    }
}
/* make hamming window multiplier */
make_hamm()
{
    int i;
	float step, arg;

	step = 2 * PI / dimension;
    arg = 0;
	for (i = 0; i < dimension; i++)
    {
		arg += step;
		/* Hamming scale */
        if (sdims->hamm)
			hamm[i] = (-(0.84 * cos(arg)) + 1) / 2.0;
		else
        	hamm[i] = 1;
	}
}
/* make the bit reverse table */
make_reverse()
{
    int i, j, dim, rev, forw;

	logdim = 0;
    dim = dimension >> 1;
    while(dim)
    {
		logdim++;
		dim >>= 1;
    }
    for (i = 0; i < dimension; i++)
    {
		forw = i;
		rev = 0;
		for (j = 1; j <= logdim; j++)
		{
		    rev  <<= 1;
		    if (forw & 0x1) rev |= 1;
		    forw >>= 1;
		}
		reverse[i] = rev;
	}
}
startunfft() {
	int i;

	clear_input();
	while ((*snum_trans)--)	/* read in next window */
	{
		for(i = 0; i < sdims->dim; i++) {
			fft_vector[i] = *(stransform++);
		} 
    	dimonly();
		un_fft();	  	    /* perform the unfft */
		save_voice();	    /* save result */
    }
}
startfft()
{
	int i;

	clear_input();
    while (load_frame())	/* read in next window */
    {
		fft();		    /* perform the fft */
		if (sdims->nopitch) {
			fft();		    /* second fft */
			filter();	    /* remove pitch information */
			un_fft();
		}
        if (sdims->rms)
			take_magnitude();   /* take magnitude (real) only */
		else
			scale_cos(); /* give cos info the overall RMS */
		onlydim();
		save_frame();	    /* save result */
    }
#if 0
		/* these are some optional functions */
			take_magnitude(); /* real/even */
			log_scale();	    /* take natural logarithm */
			un_log_scale();     /* raise e to this power */
		un_fft();
		save_voice();	    /* save result using reverse overlap addition */
		display_result(4, 0, 0);
		second_difference();
		display_result(4, 1, 2);
		remove_precision(); /* clip at +/- 127 and make integer*/
		un_fft();	    /* perform the reverse fft */
		un_fft();	    /* perform the reverse fft */
		log_scale();	    /* take natural logarithm */
		un_log_scale();     /* raise e to this power */
#endif
}
clear_input()
{
    int i;

    for (i = 0; i < dimension; i++)
    {
		in[i]  = 0;
		fft_vector[i] = 0;
        out[i] = 127;
    }
}
load_frame()
{
    int i, bytes;
    char far *raw, far *end;
	float far *real, far *imag;

    /* first shift bytes to allow for overlap */
    end = &in[overlap];
    i = dimension - overlap;
    for (raw = in; raw < end; raw++)
    {
		*raw = *(raw + i);
    }
	/* now get next set of bytes */
	for (raw = &in[overlap], bytes = 0; bytes < i; raw++, bytes++) {
		*raw = *(sstart++);
	}
	if (sstart > s_end)
    	return FALSE;
	end = &in[dimension];
    saved_rms = 0;
    for (raw = in, real = xr, imag = xi, i = 0;
		raw < end; raw++, real++, imag++, i++)
    {
	/* scale by HAMM window */
		saved_rms += *raw * *raw * 1.0;
		*real = *raw * hamm[i];
		*imag = 0.0;
	}
    saved_rms = sqrt(saved_rms / dimension); 
	return(1);
}

save_frame()
{
    int bytes;

	for (bytes = 0; bytes < sdims->dim; bytes++) {
		*(stransform++) = fft_vector[bytes];
	}
	*snum_trans += 1;
}
/* this function performs the Fast Fourier Transform */
fft()
{
    int i, j, k, kstep, s, koff, omstep, kh, omj;
	float ur, ui, vr, vi, omr, omi;

    /* put input data in bit reverse order */
    for (i = 0; i < dimension; i++)
    {
	if ((j = reverse[i]) > i)
	{
	    ur = xr[i];
	    ui = xi[i];
	    xr[i] = xr[j];
	    xi[i] = xi[j];
	    xr[j] = ur;
	    xi[j] = ui;
	}
    }
    /* first pass of butterfly */
    /* avoid multiplication since we know that W(dimension/2) = W(PI) */
    /* so all real W's are 0, and all imaginary W's are 1 or -1 */
    for (i = 0; i < dimension; i += 2)
    {
	ur = xr[i];
	vr = xr[i+1];
	xr[i] = ur + vr;
	xr[i+1] = ur - vr;
	ui = xi[i];
	vi = xi[i+1];
	xi[i] = ui + vi;
	xi[i+1] = ui - vi;
    }
    /* second pass of butterfly */
    /* avoid multiplication since we know that W(dim * .25), W(dim * .5),*/
    /* W(dim * .75) are all equal to 1, 0, or -1 */
    for (i = 0; i < dimension; i += 4)
    {
	ur = xr[i];
	vr = xr[i+2];
	xr[i] = ur + vr;
	xr[i+2] = ur - vr;
	ui = xi[i];
	vi = xi[i+2];
	xi[i] = ui + vi;
	xi[i+2] = ui - vi;
	ur = xr[i+1];
	ui = xi[i+1];
	vr = -xi[i+3];
	vi = xr[i+3];
	xr[i+1] = ur + vr;
	xr[i+3] = ur - vr;
	xi[i+1] = ui + vi;
	xi[i+3] = ui - vi;
    }
    /* now do remaining butterflies */
    kstep = 8;
    koff = 4;
    omstep = dimension / 8;
    for(s = 2; s < logdim; s++)
    {
	omj = 0;
	omr = 1.0;
	omi = 0.0;
	for (j = 0; j < koff; j++)
	{
	    for (k = j; k < dimension; k += kstep)
	    {
		kh = k + koff;
		ur = xr[kh];
		ui = xi[kh];
		vr = omr * ur - omi * ui;
		vi = omr * ui + omi * ur;
		ur = xr[k];
		ui = xi[k];
		xr[k] = ur + vr;
		xr[kh] = ur - vr;
		xi[k] = ui + vi;
		xi[kh] = ui - vi;
	    }
	    omj += omstep;
	    omr = wr[omj];
	    omi = wi[omj];
	}
	kstep *= 2;
	koff *= 2;
	omstep /= 2;
    }
}
/* this function performs the reverse FFT */
/* it is just like the FFT except the complex conjugate of W is used */
/* and the final result is scaled by 1/dimension */
un_fft()
{
    int i, j, k, kstep, s, koff, omstep, kh, omj;
	float ur, ui, vr, vi, omr, omi;

    /* put input data in bit reverse order */
    for (i = 0; i < dimension; i++)
    {
	if ((j = reverse[i]) > i)
	{
	    ur = xr[i];
	    ui = xi[i];
	    xr[i] = xr[j];
	    xi[i] = xi[j];
	    xr[j] = ur;
	    xi[j] = ui;
	}
    }
    /* first pass of butterfly */
    /* avoid multiplication since we know that W(dimension/2) = W(PI) */
    /* so all real W's are 0, and all imaginary W's are 1 or -1 */
    /* Complex conjugate does not change first pass! */
    for (i = 0; i < dimension; i += 2)
    {
	ur = xr[i];
	vr = xr[i+1];
	xr[i] = ur + vr;
	xr[i+1] = ur - vr;
	ui = xi[i];
	vi = xi[i+1];
	xi[i] = ui + vi;
	xi[i+1] = ui - vi;
    }
    /* second pass of butterfly */
    /* avoid multiplication since we know that W(dim * .25), W(dim * .5),*/
    /* W(dim * .75) are all equal to 1, 0, or -1 */
    for (i = 0; i < dimension; i += 4)
    {
	ur = xr[i];
	vr = xr[i+2];
	xr[i] = ur + vr;
	xr[i+2] = ur - vr;
	ui = xi[i];
	vi = xi[i+2];
	xi[i] = ui + vi;
	xi[i+2] = ui - vi;
	ur = xr[i+1];
	ui = xi[i+1];
	vr = xr[i+3];
	vi = xi[i+3];
	/* Complex conjugate changes second pass !*/
	xr[i+1] = ur + vi;
	xr[i+3] = ur - vi;
	xi[i+1] = ui - vr;
	xi[i+3] = ui + vr;
    }
    /* now do remaining butterflies */
    kstep = 8;
    koff = 4;
    omstep = dimension / 8;
    for(s = 2; s < logdim; s++)
    {
	omj = 0;
	/* complex conjugate does not change initial value! */
	omr = 1.0;
	omi = 0.0;
	for (j = 0; j < koff; j++)
	{
	    for (k = j; k < dimension; k += kstep)
	    {
		kh = k + koff;
		ur = xr[kh];
		ui = xi[kh];
		vr = omr * ur - omi * ui;
		vi = omr * ui + omi * ur;
		ur = xr[k];
		ui = xi[k];
		xr[k] = ur + vr;
		xr[kh] = ur - vr;
		xi[k] = ui + vi;
		xi[kh] = ui - vi;
	    }
	    omj += omstep;
	    /* Complex conjugate !*/
	    omr = wr[omj];
	    omi = -wi[omj];
	}
	kstep *= 2;
	koff *= 2;
	omstep /= 2;
    }
    /* now divide by dimension */
    for (i = 0; i < dimension ; i++)
    {
	xr[i] /= dimension;
	xi[i] /= dimension;
    }
}
take_magnitude()
{
    int i;
	float far *real, far *imag;
    real = xr;
    imag = xi;
	for (i = 0; i < dimension; i++)
    {
		*real = sqrt(((*real * *real) + (*imag * *imag)) / 2.0);
		*imag = 0.0;
		real++;
		imag++;
    }
}
scale_cos() {
	int i;
	float far *real, far *imag, scale;
    double rms, rms_cos;

	/* calculate power of total signal and cos only */
	real = xr;
	imag = xi;
	rms = rms_cos = 0;
	for (i = 0; i < dimension; i++) {
		rms += (*real * *real) + (*imag * *imag);
		rms_cos += *real * *real;
		real++;
        imag++;
	}

	/* scale cos only to total power */
	if (rms_cos) scale = rms / rms_cos;
	else scale = 0;
	real = xr;
	for (i = 0; i < dimension; i++) {
		*real = *real * scale;
		real++;
	}
}
onlydim()
{
	int x, y;
	float far *dresult, far *mdresult, far *end, sigma;
	double rms;
	long int min, max;
	int num_bytes_concidered = (dimension / 2.0);
    /* pseudo mel scale gives higher precision to low frequencies */
	int num_high_precis = (sdims->dim - 1) * .75;
	int num_low_precis = (sdims->dim - 1) - num_high_precis;
	int high_precis_bytes = 0;
    int low_precis_bytes = 0;

	if (num_high_precis)
		high_precis_bytes = (num_bytes_concidered * 0.6) / num_high_precis;
	if (!high_precis_bytes) high_precis_bytes = 1;
	if (num_low_precis)
		low_precis_bytes = (num_bytes_concidered * 0.4) / num_low_precis;
	if (!low_precis_bytes) low_precis_bytes = 1;

	dresult = &xr[0];
	end = &xr[dimension / 2];
	for (x = 1; x <= num_high_precis; x++)
	{
	    sigma = 0;
	    for (y = 0; y < high_precis_bytes; y++)
		{
			sigma += *dresult;
			dresult++;
		}
        if (high_precis_bytes) {
			sigma /= high_precis_bytes;
        }
		fft_vector[x] = sigma;
	}                                                 
	for (; x <= (sdims->dim - 1); x++)
	{
	    sigma = 0;
	    for (y = 0; y < low_precis_bytes; y++)
	    {
			sigma += *dresult;
			dresult++;
		}
        if (low_precis_bytes) {
			sigma /= low_precis_bytes;
        }
		fft_vector[x] = sigma;
	}
	rms = 0.0;
	for (x = 1; x <= (sdims->dim - 1); x++)
	{
		rms += fft_vector[x] * fft_vector[x];
	}
    if (sdims->dim - 1) {
		rms = sqrt(rms / (sdims->dim - 1));
    }
	/* then scale other dimensions to have RMS of 1*/
	if (rms) {
		rms = 1.0 / rms;
    }
	for(x = 1; x <= (sdims->dim - 1); x++)
		fft_vector[x] *= rms;
	/* store real voice RMS as 0th dimension */
	fft_vector[0] = saved_rms;
}
dimonly()
{
	/* Make Imag/Even for less noise than Real/Even */
	int x, y;
	float far *dresult, far *mdresult, far *end, sigma;
    float far *imag;
	double rms;
	long int min, max;
	int num_bytes_concidered = (dimension / 2.0);
    /* pseudo mel scale gives higher precision to low frequencies */
	int num_high_precis = (sdims->dim - 1) * .75;
	int num_low_precis = (sdims->dim - 1) - num_high_precis;
	int high_precis_bytes = 0;
    int low_precis_bytes = 0;

	if (num_high_precis)
		high_precis_bytes = (num_bytes_concidered * 0.6) / num_high_precis;
	if (!high_precis_bytes) high_precis_bytes = 1;
	if (num_low_precis)
		low_precis_bytes = (num_bytes_concidered * 0.4) / num_low_precis;
	if (!low_precis_bytes) low_precis_bytes = 1;

	/* clear all values */
	dresult = &xr[0];
    mdresult = &xi[0];
	end = &xr[dimension];
	for(;dresult < end; dresult++) {
		*dresult = 0;
		*mdresult = 0;
        mdresult++;
    }
	/* Now put back the  bands as SIN values */
#if 0
    if (sdims->rms) {
		dresult = &xi[0];
		mdresult = &xi[dimension - 1];
		end = &xi[dimension / 2];
	}
	else {
		dresult = &xr[0];
		mdresult = &xr[dimension - 1];
		end = &xr[dimension / 2];
	}
#endif
	dresult = &xi[0];
	end = &xi[dimension / 2];
	for (x = 1; x <= num_high_precis; x++)
	{
		for (y = 0; y < high_precis_bytes; y++) {
			*dresult = fft_vector[x];
			dresult++;
#if 0
			*mdresult = fft_vector[x];
			mdresult--;
#endif
	    }
	}
	for (; x <= (sdims->dim - 1); x++)
	{
		for (y = 0; y < low_precis_bytes; y++)
	    {
			*dresult = fft_vector[x];
			dresult++;
#if 0
			*mdresult = fft_vector[x];
			mdresult--;
#endif
	    }
	}
#if 0
	for (;dresult <= end;dresult++)
	{
	    *dresult = 0;
		*mdresult = 0;
	}
#endif
}
remove_precision()
{
    int i, real_char, imag_char;
	float far *real, far *imag;

    real = xr;
    imag = xi;
	for (i = 0; i < dimension; i++)
    {
		if (*real < -127)  *real = -127;
		if (*real >  127)  *real =  127;
		if (*imag < -127)  *imag = -127;
		if (*imag >  127)  *imag =  127;
		real_char = (int)*real;
		imag_char = (int)*imag;
		*real = real_char;
		*imag = imag_char;
		real++;
		imag++;
    }
}
log_scale()
{
    int i;
	float far *real, far *imag;

	real = xr;
    imag = xi;
    for (i = 0; i < dimension; i++)
    {
		if (*real <= 1) /* a no-no */
		    *real = 0;
		else
		    *real = log(*real);
		if (*real < 0) *real = 0;
		if (*imag <= 1) /* a no-no */
		    *imag = 0;
		else
		    *imag = log(*imag);
		if (*imag < 0) *imag = 0;
		real++;
		imag++;
    }
}
un_log_scale()
{
    int i;
	float far *real, far *imag;

	real = xr;
    imag = xi;
    for (i = 0; i < dimension; i++)
    {
		*real = exp(*real);
		*imag = exp(*imag);
		real++;
		imag++;
    }
}
filter()
{
	float rpitch, scale, rms;
	float far *real, far *imag, far *end, far *ham;
    int x, pitch;

	/* here the filter is a square wave passing formant */
	/* and clipping high frequency pitch */
    if (sdims->rms) {
		end  = &xr[dimension - dimension / 8];
	    real = &xr[dimension / 8];
		imag = &xi[dimension / 8];
	}
    /* double precision frequency when COS only */
	else {
		end  = &xr[dimension - dimension / 16];
	    real = &xr[dimension / 16];
		imag = &xi[dimension / 16];
	}
	for (;real < end; real++, imag++)
    {
		*real = 0.0;
		*imag = 0.0;
    }
}
second_difference()
{
    float far *real, far *end;

    end = xr;
    /* first difference */
    for(real = &xr[dimension]; real > end; real--)
    {
		*real = *real - *(real - 1);
    }
    /* second difference */
    for(real = &xr[dimension]; real > end; real--)
    {
		*real = *real - *(real - 1);
    }
}
/* Will use this section for unstransform */
save_voice()
{
    int i, bytes;
	float far *hammptr, far *raw, far *curr, far *end, max;
    double rms, scale;

	i = overlap;
	end = &out[i];
	for (raw = out; raw < end; raw++) {
		*raw = *(raw + (dimension - overlap));
    }
	/* AVERAGE WITH NEW VALUE (UNHAMMING-WINDOWED) */
	curr = xr;
    hammptr = hamm;
	for (raw = out; raw < end; raw++) {
    	if (*hammptr)
			*raw = (*raw + (*curr / *hammptr)) / 2.0;
		curr++;
        hammptr++;
    }
	/* then fill in remainder */
	end = &out[dimension];
	for (; raw < end; raw++) {
    	if (*hammptr)
			*raw = (*curr / *hammptr);
		hammptr++;
		curr++;
    }
	/* scale back to original RMS */
	raw = out;
	end = &out[dimension - overlap];
	rms = 0;
    max = 0;
	for (;raw < end; raw++) {
		rms += *raw * *raw;
		if (*raw > max) {
			max = *raw;
        }
		else if (-*raw > max) {
			max = -*raw;
        }
    }
	if (dimension - overlap) {
		rms = sqrt(rms / (dimension - overlap));
    }
	if (rms) {
		scale = fft_vector[0] / rms;
	}
	else {
		scale = 0;
	}
	/* see if current scale will blow away peak amplitude */
	if ((scale * max) > 126.0) {
    	/* if so, scale to peak amplitude for understandability */
		scale = 126.0 / max;
    }
	for (raw = out; raw < end; raw++)
	{
		*raw = scale * *raw;
	}
	raw = &out[0];
	end = &out[dimension - overlap];
	for(;raw < end; raw++) {
		*(s_end++) = *raw;
	}
}
