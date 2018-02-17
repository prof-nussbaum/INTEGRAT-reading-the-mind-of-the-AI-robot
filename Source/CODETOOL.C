#include <math.h>
#include <time.h> // for randomize()
#include <stdlib.h>
#include "FUNCTIONS.H"
#define DEBUG 0

/* the types of classification algorithms */
/* Unsupervised Competative Learning */
#define UCL 1
/* Self Organizing Map (linear) */
#define SOM 2
/* Supervised Competative Learning */
#define SCL 3
/* Differential Competative Learning */
#define DCL 4
/* Auto-associative backprop */
#define AUTOASSC 5
/* hetero-assoc. backprop */
#define BACK 6
/* number of classification types */
#define MAX_TYPES 6

/* The maximum number of dimensions in an input vector */
#define MAX_DIM 256
/* The maximum number of competative learning units */
#define MAX_UNITS 127
#define MAX_NEURONS = MAX_DIM * 2 + MAX_UNITS 
/* allocation space for coding workarea */
#define HALFMEG (long)(1024.0 * 1024.0 * 0.5)


typedef struct weight {
	float w, dw;
}WEIGHT;
typedef struct nn {
	float sum;
    float error;
	float state;
    WEIGHT bias;
}NEURON;
/* global control variables */
extern CONFIG_STRUCT config;
/* windows hook  "This Instance" */
extern HINSTANCE hInst;

// STATIC functions for just this module
static int process_units();
static int start();
static int nn_init_weights();
static float sigmoid(float x);
static float unsigmoid(float x);
static void buggo( int num, float a, float b, float c, float d);

/* some workspace to play with */
/*Data required for coding units*/
static float *units;
/*Method of classification */
static int classification;
/*drawing color, current winning classification unit, previous winning unit*/
static unsigned char shades, winning_unit_num, last_win_num;
/*pointers to above*/
static float *winning_unit, *last_win;
/*training iteration, learning constant (decays),  unused linear rate of decay (use NWE 'stabular decay)*/
static float iteration, train_rate, train_scale;
/*number of samples to code before picking an overall winner (or completing training)*/
static float this_pass;

/* input data will be stored here */
static float *dummy;

/* Statistics on code usage */
static float *stats;

/* Statistics on random sample distribution */
static float *rand_stat;

/* offset and scale tables */
static float *dim_min, *dim_max;

/* Neural Network */
static NEURON *nn, *in, *hid, *out;

/* Min/max values of input data by dimension, only chars so clips at +/-126 */

/*command information*/
/*number of dimensions in an input vector, number of coding units*/
static int num_dim, num_units;
/* number of neurons */
static long num_neurons;
/*flag to init coding units or not (form samples or random, as per coding style), Flag = 1 for classification only, 0 for class. then training*/
static int initialize, t_unit;
/* normalization of input vector flag*/
static int normalization;
/*messages*/
static char string[80];
/*data alignment test flag (1 = bad news)*/
static char screwy_flag;

/*Current random or sequential input data sample pointers*/
/*The set of sampleas with the same ID, or the test set of samples*/
static char *this_set;
/*start of selectes vector from the set*/
static float *this_vect;
/*current dimension in the vector (all dimensions copied into dummy array)*/
static float *this_dim;
/*byte after the last byte in this set*/
static char *end_set;
/*the dim after the last dim in the vector*/
static float *end_dim;
/* the number of vectors in the set*/
static unsigned long size_set;
/*The (not so) random number*/
static unsigned long random;

/*if testing, the segment to classify*/
static SEGMENT *segptr;

/* the current window */
static HWND hWnd;
/* the Tools for Coding program functions*/



/* function Prcess units
if training set number of passes through random vectors of training sets
else one pass through all vectors in segment
	test for data misalignment
	normalize input vector as necessary
	find the winning code
	decrement pass counter
	increment iteration counter if training
	get next vector to process
end for all passes
*/
static process_units()
{
    int x, y, n, wrap, cla;
    char *parse1, *parse2, c;
	HDC hdc;
	HFONT fTemp;    // Placeholder for the original font
	RECT rTemp;     // Client are needed by DrawText()
	char name[80];	//ID label
    int height;

	if (!t_unit){
		get_next_vect();
	}
	rTemp.left = 0;
	rTemp.right = 50 * 15;
	rTemp.top = 2 + (50 * (2 + .5 + 1 + .5 + 2));
	rTemp.bottom = 2 + (50 * (2 + .5 + 1 + .5 + 2 + .25));
	x = 0;
	y = 0;
    screwy_flag = FALSE;
    if (!t_unit) {
		this_pass = config.id.count;
	    /* present training set 50 times */
		this_pass *= 50;
	}
	else {
		this_pass = size_set;
	}
	while (this_pass > 0) {
		if (screwy_flag) {
			MessageBox(hWnd,"BUG: Data Misalignment","CODING ABORTED!",MB_OK);
			return (FALSE);
        }
		if (normalization){
			norm_vect();
	    }
	    classify_vector();
		if (!t_unit){
			train_unit();
	    }
		this_pass -= 1;
		config.code.iter = iteration;
		sprintf(name,"%g       ", (float)iteration);
		/* print iteration # every once in a while */
		if (DEBUG || !(config.code.iter % 100)) {
			hdc = GetDC(hWnd);
			SetTextColor( hdc, RGB(25,0,25) );
			DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER | DT_CALCRECT );
		    height = DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER);
			ReleaseDC(hWnd, hdc );
		}
		if (!t_unit)
			iteration += 1;
		get_next_vect();
	}
}
/* function initialize units
if UCL or SCL
	start code vectors as randomly selected vectors of training set
	if SCL
		make sure example is of the right ID
	end if
else
	randomly set weights
end if
*/
/*
    Initialize a new set of units
if UCL or SCL
	Init with random samples from the training set
	if SCL
		make sure init has same ID as code unit
	end if
end if
if BACK or AUTOASSC
	init weights of the NN
end if
*/
init_units()
{
	float *unit_dim;

	int n, d, u, x, y;
    unsigned long z;

	/* clear out dimensional range +/-126 arrays */
	for (x = 0; x < config.tran.dim; x++) {
		dim_min[x] = 126;
		dim_max[x] = -126;
    }
	for (x = config.id.count; x > 0;) {
		this_set = end_set;
		if (this_set >= config.file.tra.cur_pos)
        	this_set = config.file.tra.data;
		size_set = *(float *)(this_set + 4);
		this_vect = (float *)(this_set +
				(4 * sizeof(char)) + sizeof(float));
		end_set = (char *)(this_vect +
				(size_set * num_dim));
		if (!((*(this_set + 2) == 'C') && (*(this_set + 3) == 'K'))) {
			//something is screwy
			screwy();
        }
		this_dim = this_vect;
		end_dim = this_vect + num_dim;
		d = size_set;
		for (;d ;d--) {
			for (y = 0; y < num_dim; y++) {
            	dummy[y] = *this_dim++;
	    		if (dummy[y] < dim_min[y])
					dim_min[y] = dummy[y] - 0.2;
				if (dummy[y] > dim_max[y])
					dim_max[y] = dummy[y] + 0.2;
				if (norm_dim(dummy[y], y) > 1)
					screwy();
				if (norm_dim(dummy[y], y) < 0)
					screwy();
			}
			z = (unsigned long)(   ((char *)this_vect -
								config.file.tra.data - 8) /
							(num_dim * sizeof(float))    );
			if (&rand_stat[z] < dim_min){
				rand_stat[z]++;
			}
			this_vect = end_dim;
			this_dim = this_vect;
			end_dim = this_vect + num_dim;
            x--;
		}
	}
	if ((classification == SCL) || (classification == UCL))
    {
		u = 1;
	    unit_dim = units;
		while (u <= num_units){
			get_next_vect();
			if (classification == SCL) {
				/* would like to init with member of it's own set */
	            /* but give up after 500 tries */
	        	d = 500;
				while ((*this_set != u) &&
						(*(this_set + 1) != u)) {
					get_next_vect();
					if (!d--) break;
				}
				if (!d)
					;//screwy();/*should have found one*/
			}
			if (!((*(this_set + 2) == 'C') && (*(this_set + 3) == 'K'))) {
				//something is screwy
				screwy();
	        }
			for (d = 0; d < num_dim; d++){
			    *unit_dim++ = dummy[d];
			}
			u++;
		}
	}
	else /* BACK OR AUTOASSC */
	{
		nn_init_weights();
    }
}


setup_ram()
{
	units = (float *)config.file.cod.data;
	config.file.cod.cur_pos = (char *)units;
	config.file.cod.cur_pos +=
		(long)(1.0 * sizeof(float) * num_dim * num_units);
	dummy = (float *)config.file.cod.cur_pos;
	config.file.cod.cur_pos +=
		(long)(1.0 * sizeof(float) * num_dim);
	stats = (float *)config.file.cod.cur_pos;
	config.file.cod.cur_pos +=
		(long)(1.0 * sizeof(float) * num_units);
	rand_stat = (float *)config.file.cod.cur_pos;
	config.file.cod.cur_pos +=
		(long)(1.0 * sizeof(float) * config.id.count);
	dim_min = (float *)config.file.cod.cur_pos;
	config.file.cod.cur_pos +=
		(long)(1.0 * sizeof(float) * config.tran.dim);
	dim_max = (float *)config.file.cod.cur_pos;
	config.file.cod.cur_pos +=
		(long)(1.0 * sizeof(float) * config.tran.dim);
	nn = (NEURON *)config.file.cod.cur_pos;
	in = nn;
	config.file.cod.cur_pos +=
		(long)(1.0 * (sizeof(NEURON) * num_dim));
	hid = (NEURON *)config.file.cod.cur_pos;
	config.file.cod.cur_pos +=
		(long)(1.0 * ((sizeof(NEURON) * num_units) +
					(num_units * sizeof(WEIGHT) * num_dim)));
	out = (NEURON *)config.file.cod.cur_pos;
    if (classification == AUTOASSC) {
		config.file.cod.cur_pos +=
			(long)(1.0 * ((sizeof(NEURON) * num_dim) +
					(num_dim * sizeof(WEIGHT) * num_units)));
    }
	else {
		config.file.cod.cur_pos +=
			(long)(1.0 * ((sizeof(NEURON) * num_units) +
					(num_units * sizeof(WEIGHT) * num_units)));
    }
	config.file.cod.size = config.file.cod.cur_pos -
		config.file.cod.data;

    if (config.file.cod.size > HALFMEG) {
			MessageBox(hWnd,"Halfmeg too small","Cannot Do Coding",MB_OK);
			return FALSE;
	}
	else
    	return TRUE;
}


/*
    Display statistics
	here each vector is drawn, with a background color proportional to
	the number of times the vector was used.
    NOTE that only the second most used vector is considered as the
    most used, because the most used vector will be the silence
	vector.

*/
display_stats()
{
	float max, max2;
	int dim, max_dim, max_dim2;
	max = max2 = 0;
    max_dim = max_dim2 = 0;

	if (t_unit) /* testing pass */
	{
		for (dim = 0; dim < num_units; dim++) {
			if (stats[dim] > max) {
				max2 = max;
				max = stats[dim];
                max_dim2 = max_dim;
				max_dim = dim + 1;
			}
		}
		if (max) {
			segptr->class1 = max_dim;
			if (config.id.pair && max2) {
				segptr->class2 = max_dim2;
			}
			else {
				segptr->class2 = 0;
            }
		}
		else {
			segptr->class1 = 0;
			segptr->class2 = 0;        	
		}
		drawseg(hWnd);
        drawcodestat(hWnd);/* draw code firing rates */
	}
	else {
		if ((classification == BACK) || (classification == AUTOASSC)){
			nnestcode();
        }
		drawcode(hWnd);
	}
}



/* static Function start
Change cursor to show user training or testing
Initialize units as necessary, display start state
Begin Coding Processing
Display all winning codes for this set or all sets (testing or training), and random vector selection statistics.
restore cursor to previous state
*/
static start()
{
    int first = 1;
    int x;
	char *c;
    HCURSOR hcurSave;

	if (t_unit) {/* training */
		hcurSave = SetCursor(LoadCursor(hInst, "CUR_CODE"));
	}
	else {
		hcurSave = SetCursor(LoadCursor(hInst, "CUR_TRAI"));
	}
	if (first && initialize){
    	screwy_flag = FALSE;
		init_units();
        if (screwy_flag) {
			MessageBox(hWnd,"Data Misalignment","INITIALIZATION ABORTED!",MB_OK);
			return (FALSE);
        }
		display_stats();
		first = 0;
	}/* end if initializing */
	process_units();
	display_stats();
    SetCursor(hcurSave);
}


/* Entry Function void codenow()
CONFIG_STRUCT config must have been filled in correctly
CODE ON is true
FILE COD DATA exists
ID COUNT coding scale set
CODE one of UCL, SCL, AUTOASSC, BACK
TRAN DIM valid range
parameter seg must have data to test if testing
then
determine if this is the first pass (can't do testing yet)
load config into static copies
parse out workspace RAM (check for enough room)
clear out winning unit array
clear out randomiztion visual qualitative test area
clear min/max array
setup first set to process
start coding	
*/
codenow(HWND hwnd, int test, SEGMENT *seg)
{
	long int x, y;
    float *dptr;

    hWnd = hwnd;
	/* Test for valid configuration */
	if (!((config.code.on) &&
			(config.file.cod.data) &&
			(config.id.count)	)) {
		//MessageBox(hWnd,"Coding Not On","Cannot Do Coding",MB_OK);
		return FALSE;
	}//must have workspace
	if (!(config.code.ucl || config.code.scl || config.code.autoassc || config.code.back)) {
		MessageBox(hWnd,"Coding method not selected","Cannot Do Coding",MB_OK);
		return FALSE;
	}//must be correct coding selection
	if (!config.tran.dim) {
		MessageBox(hWnd,"Invalid Transform Resultant Dimension","Cannot Do Coding",MB_OK);
		return FALSE;
	}//must have meaningful dimensions
	if (config.tran.dim > MAX_DIM) {
		MessageBox(hWnd,"Transform Dim too Large","Cannot Do Coding",MB_OK);
		return FALSE;
	}//must have meaningful dimensions

	t_unit = test;
	/*t_unit = 1 for classification only, 0 for training*/
	if (test && (!seg || !seg->tran_start)) {
		MessageBox(hWnd,"Test segment invalid","Cannot Do Coding",MB_OK);
		return FALSE;
	}
	else segptr = seg;


	/* arrange data in workspace */
	if (!config.file.con.size || !config.code.iter){
		config.code.iter = 0;
		if (test) {
			MessageBox(hWnd,"Cannot Test without Training","Cannot Do Coding",MB_OK);
			return FALSE;
		}
	}
    iteration = config.code.iter + 1;
	num_dim = config.tran.dim;
	num_units = config.id.num;
	if (!num_units) num_units = MAX_UNITS;
	if (!setup_ram())
		return FALSE;
    /*clear out statistics array */
    for (x = 0; x < num_units; x++){
		stats[x] = 0.0;
	}
	/*clear out rand array */
    for (x = 0; x < config.id.count; x++){
		rand_stat[x] = 0.0;
	}
	if (iteration == 1) {
		initialize = 1;
		// randomize();
		random = rand();
    }
    else {
		initialize = 0;
	}
    if (!t_unit) {/* if training */
		//let first sample be initial setting for random sequence
		this_set = config.file.tra.data;
		this_vect = (float *)(this_set +
						(4 * sizeof(char)) + sizeof(float));
		this_dim = this_vect;
	    size_set = *(float *)(this_set + 4);
		end_set = (char *)(this_vect +
						(size_set * num_dim));
		end_dim = this_vect + num_dim;
        dptr = dummy;
		for (y = 0; y < num_dim; y++) {
			*dptr = norm_dim(*dptr, y);
            dptr++;
		}
	}
	else {
		this_set = (char *)segptr->tran_start;
		this_vect = (float *)this_set;
		this_dim = this_vect;
		size_set = (float)segptr->num_trans;
		end_set = (char *)(this_vect +
					(size_set * num_dim));
		end_dim = this_vect + num_dim;
        dptr = dummy;
		for (y = 0; y < num_dim; y++) {
			*dptr = norm_dim(*dptr, y);
            dptr++;
		}
	}
	if (config.code.ucl)
		classification = UCL;
	else if (config.code.scl)
		classification = SCL;
	else if (config.code.autoassc)
		classification = AUTOASSC;
	else
		classification = BACK;
	normalization = 0;//no normalization

	train_rate = (float)(config.id.count * 25.0);

    // randomize();
	start();
}

/* init weights of the NN */
/* weights set to +/- .1, other values cleared */
static nn_init_weights()
{
	NEURON *n;
	WEIGHT *w;
    float noise;
	int neuron, weight, num_out;

	/* input layer */
	n = in;
	for (neuron = 0; neuron < num_dim; neuron++) {
		n->state = 0.0;
		n->sum = 0.0;
		n->error = 0.0;
        n++;
	}
	/* hidden layer */
	n = hid;
	for (neuron = 0; neuron < num_units; neuron++) {
		n->state = 0.0;
		n->sum = 0.0;
		n->error = 0.0;
    	//bias added
		noise = (rand() % 99) - 49.0;
		if (noise) noise = (noise / 490.0);
		else noise = 0;
		n->bias.w = noise;
		n->bias.dw = 0.0;
		w = (WEIGHT *)((char *)n + sizeof(NEURON));
		for (weight = 0; weight < num_dim; weight++) {
			noise = (rand() % 99) - 49.0;
			if (noise) noise = (noise / 490.0);
			else noise = 0;
			w->w = noise;
			w->dw = 0.0;
			w++;
		}
        n = (NEURON *)w;
	}
	/* output layer */
	n = out;
	if (classification == BACK) num_out = num_units;
    else num_out = num_dim;
	for (neuron = 0; neuron < num_out; neuron++) {
		n->state = 0.0;
		n->sum = 0.0;
		n->error = 0.0;
		//bias added
		noise = (rand() % 99) - 49.0;
		if (noise) noise = (noise / 490.0);
		else noise = 0;
		n->bias.w = noise;
		n->bias.dw = 0.0;
		w = (WEIGHT *)((char *)n + sizeof(NEURON));
		for (weight = 0; weight < num_units; weight++) {
			noise = (rand() % 99) - 49.0;
			if (noise) noise = (noise / 490.0);
			else noise = 0;
			w->w = noise;
			w->dw = 0.0;
			w++;         	
		}
        n = (NEURON *)w;
	}
}
/*
    Classify the input vector

    UCL SCL - choose the unit whose vector is closest to the input vector
		Save the previous winning unit
        take euclid distance between each code and this vector 
		for all the dimensions except the first (RMS)
		winning code is min euclid dist. or first if tie
    BACK AUTOACCS - forward pass of NN
*/
classify_vector()
{
    int u, d;
	float euclid_dist, dim_dist, min_dist;
	float *unit_dim, *this_unit;

	if (!((classification == SCL) || (classification == UCL))){
		nn_class_vect();
	}
    else {/* UCL or SCL */
		min_dist = 99999999999;
		unit_dim = units;
		this_unit = unit_dim;
		last_win_num = winning_unit_num;
	    last_win = winning_unit;
		winning_unit = units;
	    winning_unit_num = 1;
	    for (u = 1; u <= num_units; u++){
			euclid_dist = 0;
			for (d = 0; d < num_dim; d++){
				dim_dist = *unit_dim - dummy[d];
				/* ignore RMS in distance calc */
	            if (d == 0)
					;
				else
					euclid_dist += (dim_dist * dim_dist);
	    		unit_dim++;
			}
			euclid_dist = sqrt(euclid_dist);
			/* only one winner! Not very parallel! */
			if (!t_unit) {/* no concience */
				if ((euclid_dist /* * stats[u-1] */ )
						< min_dist){
				    min_dist = euclid_dist;
					winning_unit = this_unit;
	        	    winning_unit_num = u;
				}
			}
	        else {
				if (euclid_dist < min_dist){
				    min_dist = euclid_dist;
					winning_unit = this_unit;
	        	    winning_unit_num = u;
				}
			}
			this_unit = unit_dim;
		}
		if (this_unit != dummy)
			screwy();/* traversal of units not aligned !*/
    }/* end if SCL or UCL */
    /* keep track of statistics */
    stats[winning_unit_num - 1] += 1;
}
/* function nn class nvect
forward pass of NN
AUTOASS has output as winner of middle layer
BACK has winner at output layer
*/
nn_class_vect()
{
	NEURON *n, *othern;
	WEIGHT *w;
	float noise, max, total;
	int neuron, weight, num_out, x;
	float *dptr;

	max = 0.0;
    winning_unit_num = 1;
	/* input layer gets sample vector */
	n = in;
    dptr = dummy;
	for (neuron = 0; neuron < num_dim; neuron++) {
		n->state = *dptr;
		n++;
        dptr++;
	}
	/* hidden layer */
	n = hid;
    total = 0.0;
	for (neuron = 0; neuron < num_units; neuron++) {
		n->state = 0.0;
		n->sum = 0.0;
		n->error = 0.0;
		//bias added
		n->sum += n->bias.w * 0.8;
		dptr = dummy;
		w = (WEIGHT *)((char *)n + sizeof(NEURON));
		for (weight = 0; weight < num_dim; weight++) {
			if (DEBUG)
            	buggo( 1, n->sum, dummy[weight], w->w, 0.0);
			n->sum += *dptr * w->w;
			w++;
            dptr++;
		}
		n->state = sigmoid(n->sum);
		if (classification == AUTOASSC) {
        	total += n->state;
			if (n->state > max) {
				max = n->state;
				winning_unit_num = neuron + 1;
			}
        }
		n = (NEURON *)w;
	}
	/* force winner take all */
	if (classification == AUTOASSC) {
		n = hid;
		for (neuron = 0; neuron < num_units; neuron++) {
			if (neuron + 1 != winning_unit_num)
				n->state = 0.0;
			n = (NEURON *)((char *)n +
			   sizeof(NEURON) + (num_dim * sizeof(WEIGHT)));
        }
    }
	/* output layer */
	n = out;
	if (classification == BACK) num_out = num_units;
    else num_out = num_dim;
	for (neuron = 0; neuron < num_out; neuron++) {
		n->state = 0.0;
		n->sum = 0.0;
		n->error = 0.0;
		//bias added
		n->sum += n->bias.w * 0.8;
		w = (WEIGHT *)((char *)n + sizeof(NEURON));
		othern = hid;
		for (weight = 0; weight < num_units; weight++) {
			if (DEBUG)
            	buggo( 2, n->sum, othern->state, w->w, 0.0);
			n->sum += othern->state * w->w;
			w++;
			othern = (NEURON *)((char *)othern +
               sizeof(NEURON) + (num_dim * sizeof(WEIGHT)));
		}
		n->state = sigmoid(n->sum);
		if (classification == BACK)
			if (n->state > max) {
				max = n->state;
				winning_unit_num = neuron + 1;
			} 
//		states[neuron] = n->state;
		n = (NEURON *)w;
	}
}
static float sigmoid(float x) {
	double dx;
	dx = x;
	if (DEBUG)
       	buggo( 3, x, (float)dx, 0.0, 0.0);
	if (dx < -10) return(0.0);
	if (dx > 10) return(1.0);
	x = 1.0 / (1.0 + exp(-dx)) ;
	return(x);
}
static float unsigmoid(float x) {
	double dx;
	dx = x;
	if (DEBUG)
       	buggo( 3, x, (float)dx, 0.0, 0.0);
	if (dx < (1.0 - 0.999955)) return(-10.0);
	if (dx > 0.999955) return(10.0);
	x = log((1.0 / dx) - 1);
	return(x);
}
/* function train unit
calculate train_scale using table of rate decay
if AUTOASSC or BACK
	do backward pass on NN
if UCL or SOM (not impl.)
	reduce distance of vector to example by train_scale
if SCL
	if correct winner (could have two chances if using PAIR)
		reduce distance of vector by train_scale
    else
		increase distance of vector by train_scale
		make sure all dims stay within bounds of real data
	end if
end if
    	
*/
train_unit()
{
	int u, d, cla, trap;
	float *unit_dim;
    unsigned long count = config.id.count;

    /* linear decay of learning */
	/*train_scale = train_rate / (iteration + train_rate);*/
	/* use same decay as NWE Explorer default learning schedule */
	if (iteration < 100 * count)
		train_scale = 0.5;
	else if (iteration < 200 * count)
		train_scale = 0.1;
	else if (iteration < 300 * count)
		train_scale = 0.05;
	else if (iteration < 1000 * count)
		train_scale = 0.01;
	else if (iteration < 2000 * count)
		train_scale = 0.0075;
	else if (iteration < 3000 * count)
		train_scale = 0.002;
	else
		train_scale = 0.001;
	unit_dim = winning_unit;
	if ((classification == BACK) || (classification == AUTOASSC)){
    	nn_train();
    }
	if ((classification == UCL) || (classification == SOM)){
	  /* Differential competative learning only train on delta win */
	  /* if (last_win != winning_unit) {*/
      if (1) {
		for (d = 0; d < num_dim; d++){
			*unit_dim +=
			train_scale * (dummy[d] - *unit_dim);
			unit_dim++;
		}
		if ((dummy[0] < 5) || (dummy[0] > 100))
			trap = 5;
      }
#if 0
		//SOM not implemented	
		if (classification == SOM){
		    /* train neighbor units similarly */
		    if (winning_unit > 0){
				for (d = 0; d < num_dim; d++){
				    units[winning_unit - 1].vector[d] +=
					(train_scale/2) *
					(dummy[d] - units[winning_unit-1].vector[d]);
				}
		    }
		    if (winning_unit < num_units - 1){
				for (d = 0; d < num_dim; d++){
				    units[winning_unit + 1].vector[d] +=
					(train_scale/2) *
					(dummy[d] - units[winning_unit + 1].vector[d]);
				}
		    }
		}/* end if SOM */
#endif
    }/* end if UCL or SOM */
	if (classification == SCL){
		if ((winning_unit_num == *this_set) ||
			(winning_unit_num == *(this_set + 1)))
		{
			unit_dim = winning_unit;
		    for (d = 0; d < num_dim; d++)
		    {
				*unit_dim +=
				train_scale * (dummy[d] - *unit_dim);
				/* no need to test new dim,
				it moved closer to real data*/
				unit_dim++;
		    }
		}
		else
		{
			unit_dim = winning_unit;
		    for (d = 0; d < num_dim; d++)
			{
				*unit_dim -=
					(train_scale / num_units) *
					//train_scale *
					(dummy[d] - *unit_dim);
				/* check new dimension for valid range */
#if 0
				/* if ouside range, wrap around */
				if (*unit_dim > dim_max[d])
					*unit_dim = dim_min[d];
				else if (*unit_dim < dim_min[d])
					*unit_dim = dim_max[d];
				/* hard limit instead of wraparound */
				if (*unit_dim > dim_max[d])
					*unit_dim = dim_max[d];
				else if (*unit_dim < dim_min[d])
					*unit_dim = dim_min[d];
#endif

				unit_dim++;
		    }
		}
	}/* end if SCL */
	if ((classification == BACK) || (classification == SCL)){
		if (*this_set)
			for(d = 0;d < count / num_units; d++) {
				if (DEBUG)
    	        	buggo( 4, (float)*this_set, (float)count, (float)num_units, (float)d);
				rand_stat[((*this_set - 1) *
					(count / num_units)) + d]++;
            }
		if (*(this_set + 1))
			for(d = 0;d < count / num_units; d++) {
				if (DEBUG)
    	        	buggo( 5, (float)*(this_set + 1), (float)count, (float)num_units, (float)d);
				rand_stat[((*(this_set + 1) - 1) *
					(count / num_units)) + d]++;
            }
	}
	return(trap);
}
nn_train() {
	NEURON *n, *othern;
	WEIGHT *w;
	float noise, max;
	float states[127];
	float errors;
    float clamp;
	int neuron, weight, num_out;
    float *dptr;

	max = 0.0;
    clamp = 0.00001;

	errors = 0.0;
	/* backprop error according to model type */
    if (classification == BACK) {
		/* output layer */
		n = out;
		num_out = num_units;
		for (neuron = 0; neuron < num_out; neuron++) {
			if (((neuron + 1) == *this_set) ||
				((neuron + 1) == *(this_set + 1))) {
				/* should have been on */
                
				if (DEBUG)
    	        	buggo( 6, (float)n->state, (float)n->sum, 0.0, 0.0);
				n->error = (0.8 - n->state);
			}
			else /* should have been off */ {
				if (DEBUG)
    	        	buggo( 7, (float)n->state, (float)n->sum, 0.0, 0.0);
				n->error = (0.2 - n->state);
			}
			states[neuron] = n->state;
			errors += (n->error * n->error);
			/*scale by derivative of xfer func*/
			n->error *= n->state * (1.0 - n->state);
			if ((n->error < clamp) && (n->error > -clamp)){
				n->error = 0.0;
            }
			n = (NEURON *)((char *)n +
			   sizeof(NEURON) + (num_units * sizeof(WEIGHT)));
		}
	}
    else /* AUTOASSOC Should output same as input */ {
		/* output layer */
		n = out;
		num_out = num_dim;
        dptr = dummy;
		for (neuron = 0; neuron < num_out; neuron++) {
			if (DEBUG)
				buggo( 8, (float)n->state, (float)n->sum, dummy[neuron], 0.0);
			n->error = (*dptr - n->state);
			if (*dptr > 1.0)
				//screwy(); /* let autoassc create unbounded centroids*/
			errors += (n->error * n->error);
			n->error *= n->state * (1.0 - n->state);
			if ((n->error < clamp) && (n->error > -clamp)) {
				n->error = 0.0;
			}  
			n = (NEURON *)((char *)n +
			   sizeof(NEURON) + (num_units * sizeof(WEIGHT)));
            dptr++;
		}
	}
	errors *= 0.5;
	/* now that errors are calculated for output layer, change weights */
	n = out;
	for (neuron = 0; neuron < num_out; neuron++) {
		//added bias
		if ((n->bias.dw < clamp) && (n->bias.dw > -clamp)) {
			n->bias.dw = 0.0;
		}  
		n->bias.dw = (train_scale * n->error * 0.8) +
				(0.4 * n->bias.dw);
		n->bias.w += n->bias.dw;
		w = (WEIGHT *)((char *)n + sizeof(NEURON));
		othern = hid;
		for (weight = 0; weight < num_units; weight++) {
			/* stuff backprop error into hidden layer*/
			if (DEBUG)
   	        	buggo( 9, (float)n->error, (float)w->w, 0.0, 0.0);
			othern->error += n->error * w->w;
			/* calculate new weight from this layer's error*/  
			if ((w->dw < clamp) && (w->dw > -clamp)) {
				w->dw = 0.0;
			}  
			if (DEBUG)
				buggo( 10, train_scale, (float)n->error, othern->state, w->dw);
			w->dw = (train_scale * n->error * othern->state) +
					(0.4 * w->dw);
			if (DEBUG)
   	        	buggo( 11, w->w, w->dw, 0.0, 0.0);
			w->w += w->dw;
			w++;
			othern = (NEURON *)((char *)othern +
			   sizeof(NEURON) + (num_dim * sizeof(WEIGHT)));
		}
		n = (NEURON *)w;
	}
	/* hidden layer */
	n = hid;
	for (neuron = 0; neuron < num_units; neuron++) {
		n->error *= n->state * (1.0 - n->state);
		if ((n->error < clamp) && (n->error > -clamp)) {
			n->error = 0.0;
		}  
		//added bias
		if ((n->bias.dw < clamp) && (n->bias.dw > -clamp)) {
			n->bias.dw = 0.0;
		}  
		n->bias.dw = (train_scale * n->error * 0.8) +
				(0.4 * n->bias.dw);
		n->bias.w += n->bias.dw;
		w = (WEIGHT *)((char *)n + sizeof(NEURON));
        dptr = dummy;
		for (weight = 0; weight < num_dim; weight++) {
			if ((w->dw < clamp) && (w->dw > -clamp)) {
				w->dw = 0.0;
			}  
			if (DEBUG)
   	        	buggo( 12, train_scale, (float)n->error, dummy[weight], w->dw);
			w->dw = (train_scale * n->error * *dptr) +
					(0.4 * w->dw);
			if (DEBUG)
   	        	buggo( 13, w->w, w->dw, 0.0, 0.0);
			w->w += w->dw;
			w++;
            dptr++;
		}
        if (classification == AUTOASSC) {
			states[neuron] = n->state;
		}
		n = (NEURON *)w;
	}
	drawstates(hWnd, &states[0], errors);
	/* input layer gets no training */
}


/* nn estimate code vector */
nnestcode()
{
	NEURON *n, *othern;
	WEIGHT *w, hw;
    float *dim = units; 
	int neuron, hneuron, weight, hweight, num_out;

	/* estimate code according to model type */
	if (classification == BACK) {
    	/* pretend nn is in reverse */
		n = out;
		for (neuron = 0; neuron < num_units; neuron++) {
			w = (WEIGHT *)((char *)n + sizeof(NEURON));
			othern = hid;
			for (weight = 0; weight < num_units; weight++) {
				/* work backwards to hidden layer*/
				othern->state = w->w;
				w++;
				othern = (NEURON *)((char *)othern +
				   sizeof(NEURON) + (num_dim * sizeof(WEIGHT)));
			}
            /* clear out estimated code */
			for (hneuron = 0; hneuron < num_dim; hneuron++)
				dummy[hneuron] = 0.0;
			/* calc est code from hidden states/weights */
			othern = hid;
			for (hneuron = 0; hneuron < num_units; hneuron++) {
				w = (WEIGHT *)((char *)othern + sizeof(NEURON));
				for (weight = 0; weight < num_dim; weight++) {
					/* work backwards to output layer*/
					dummy[weight] += othern->state * w->w;
					w++;
                }
				othern = (NEURON *)((char *)othern +
				   sizeof(NEURON) + (num_dim * sizeof(WEIGHT)));
			}
            /* put into code estimate area */
			for (hneuron = 0; hneuron < num_dim; hneuron++)
				*dim++ = sigmoid(dummy[hneuron]);
            /* next ouput possibility */
			n = (NEURON *)((char *)n +
			   sizeof(NEURON) + (num_units * sizeof(WEIGHT)));
		}
	}
	else if (classification == AUTOASSC) {
		/* single pass to calculate ouput */
		for (hneuron = 0; hneuron < num_units; hneuron++) {
			n = out;
			for(neuron = 0; neuron < num_dim; neuron++) {
				w = (WEIGHT *)((char *)n + sizeof(NEURON));
                w += hneuron;
				*(dim++) = sigmoid((w->w * 1.0) +
									(n->bias.w * 0.8));
				n = (NEURON *)((char *)n +
				   sizeof(NEURON) + (num_units * sizeof(WEIGHT)));
			}
		}
	}
}

/*
    If requested, this function will normalize each input
	    vector so they all have the same max amplitude
*/
norm_vect()
{
	float rms, scale;
    char newval;
    int x;

    rms = 0;
    for (x = 0; x < num_dim; x++){
	rms += dummy[x] * dummy[x];
    }
    rms = sqrt(rms / num_dim);
    if (rms){
	scale = 127 / rms;
	for (x = 0; x < num_dim; x++){
	    newval = dummy[x] * scale;
	    dummy[x] = newval;
	}
    }
    else {
	for (x = 0; x < num_dim; x++){
	    dummy[x] = 0;
	}
    }
}

get_next_vect() {
	unsigned long x, y;
	double noise;
    float *dptr;

	if (t_unit) {/* testing pass */
		this_vect = end_dim;
		this_dim = this_vect;
		end_dim = this_vect + num_dim;
		if (this_vect >= (float *)end_set)
			screwy(); /* out of segment bounds */
        dptr = dummy;
		for (y = 0; y < num_dim; y++) {
	    	*(dptr++) = *(this_dim++);
		}
	}/* end if simply select next segment vector */
	else {
		random += rand();
		random -= rand();
		random ^= rand();
		x = (random % 34) + 1; 
		for (; x; x--) {
        	random ^= rand();
			this_set = end_set;
			if (this_set >= config.file.tra.cur_pos)
	        	this_set = config.file.tra.data;
			size_set = *(float *)(this_set + 4);
			this_vect = (float *)(this_set +
					(4 * sizeof(char)) + sizeof(float));
			end_set = (char *)(this_vect +
					(size_set * num_dim));
			if (!((*(this_set + 2) == 'C') && (*(this_set + 3) == 'K'))) {
				//something is screwy
				screwy();
	        }
	    }
		//now pick a vector
		this_dim = this_vect;
		end_dim = this_vect + num_dim;
		y = random % size_set;
		for (;y ;y--) {
        	random ^= rand();
			this_vect += num_dim;
			this_dim = this_vect;
			end_dim = this_vect + num_dim;
		}
		x = (unsigned long)(   ((char *)this_vect -
								config.file.tra.data - 8) /
							(num_dim * sizeof(float))    );
		if ((classification != BACK) && (classification != SCL)){
	        if (&rand_stat[x] < dim_min){
				rand_stat[x]++;
			}
		}
        dptr = dummy;
		for (y = 0; y < num_dim; y++) {
	    	*(dptr++) = *(this_dim++);
			random += rand();
			random -= rand();
		}
	}/* end for random training vector selection */
	/* offset and scale */
	dptr = dummy;
    for (x = 0; x < num_dim; x++) {
		*dptr = norm_dim(*dptr, x);
		/* NEW Add noise to input data */
		/* 4 dice gaussian */
		noise = 0.0;
		for (y = 0; y < 4; y++) {
			noise += (random ^= rand()) % 100;
		}
		random += rand();
        random -= rand();
		noise -= 198;
		noise /= 198000; /* +/- .001 */
		*(dptr++) += noise;
	}

}
screwy()
{
	random += rand();
	screwy_flag = TRUE;
}
static void buggo( int num, float a, float b, float c, float d)
{
	HDC hdc;
	HFONT fTemp;    // Placeholder for the original font
	RECT rTemp;     // Client are needed by DrawText()
	char name[255];	//ID label
    int height;

	rTemp.left = 0;
	rTemp.right = 50 * 15;
	rTemp.top = 2 + (50 * (2 + .5 + 1 + .5 + 2));
	rTemp.bottom = 2 + (50 * (2 + .5 + 1 + .5 + 2 + .25));

	sprintf(name, "buggo %d, %g, %g, %g, %g",num, a, b, c, d);
	hdc = GetDC(hWnd);
	SetTextColor( hdc, RGB(25,0,25) );
	DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER | DT_CALCRECT );
    height = DrawText( hdc, name, -1, (RECT FAR*)&rTemp, DT_CENTER);
	ReleaseDC(hWnd, hdc );
}
float norm_dim(float x, int y)
{
	if (dim_max[y] - dim_min[y] > 0)
		return ((x - dim_min[y]) / (dim_max[y] - dim_min[y]));
	else
		return(0);
}  
float unnorm_dim(float x, int y)
{
	if (!setup_ram())
		return(0);
	if (dim_max[y] - dim_min[y] > 0)
		return ((x * (dim_max[y] - dim_min[y])) + dim_min[y]);
	else
		return(0);
}  

