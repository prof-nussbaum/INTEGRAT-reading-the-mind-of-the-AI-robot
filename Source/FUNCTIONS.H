#include "CMDLG.H"
// All of the functions within INTEGRAT
// Organized by module
//
//			Module CODE.C
//
int codefile(HWND hWnd);
int codeon(HWND hWnd);
int codeucl(HWND hWnd);
int codeautoassc(HWND hWnd);
int codescl(HWND hWnd);
int codeback(HWND hWnd);
//
//			Module CODETOOL.C 
//
// STATIC functions for just this module
//		static int process_units();
//		static int start();
//		static int nn_init_weights();
//		static float sigmoid(float x);
//		static float unsigmoid(float x);
//		static void buggo( int num, float a, float b, float c, float d);
int init_units();
int setup_ram();
int display_stats();
int codenow(HWND hwnd, int test, SEGMENT *seg);
int classify_vector();
int nn_class_vect();
int train_unit();
int nn_train();
int nnestcode();
int norm_vect();
int get_next_vect();
int screwy();
float norm_dim(float x, int y);
float unnorm_dim(float x, int y);
//
//Module CONBOX.C 
//
// STATIC functions for just this module
//	static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmconbox();
//
//Module DIALOG.C 
//
// STATIC functions for just this module
//  static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmAbout(void);
//
//Module DRAW.C 
//
int drawnot(HWND hWnd);
int drawnow(HWND hWnd);
int drawbw(HWND hWnd);
int drawall(HWND hwnd);
int drawseg(HWND hwnd);
int clearsegedit(HWND hwnd);
int clearid(HWND hwnd);
int insegwin(unsigned int x, unsigned int y);
char *getwavptr(unsigned int x);
int inwavwin(unsigned int x, unsigned int y);
int drawsegedit(HWND hwnd, unsigned int x) ;
int drawwav(HWND hwnd);
int drawid(SEGMENT *segptr, unsigned int xpos1, unsigned int xpos2, HDC hdc);
int drawtran(HWND hwnd);
int drawonetran(HWND hwnd, SEGMENT *segptr);
int drawcode(HWND hwnd);
int drawcodestat(HWND hwnd);
int shownot(HWND hwnd);
int drawstates(HWND hwnd, float *states, float errors);
//
//Module FFT.C 
//
// STATIC functions for just this module
//		static int getram();
//		static void freeram();
char *fftuntran(float *tran, int num_trans,
		  char 	*voice, TRAN_STRUCT *dims);
float *ffttran(char *start,
					char *end,
					float *transform,
					int *num_trans,
					TRAN_STRUCT *dims);
int get_command();
int make_w();
int make_hamm();
int make_reverse();
int startunfft() ;
int startfft();
int clear_input();
int load_frame();
int save_frame();
int fft();
int un_fft();
int take_magnitude();
int scale_cos();
int onlydim();
int dimonly();
int remove_precision();
int log_scale();
int un_log_scale();
int filter();
int second_difference();
int save_voice();
//
//Module FILE,C 
//
// STATIC functions for just this module
//		static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
int fileopen(HWND hWnd);
int filecon(HWND hWnd);
int fileplay(HWND hWnd);
int filerec(HWND hWnd);
int cmrambox(HWND hWnd);
int validate_names();
unsigned long load_up_file(HWND hWnd, MEMORY_FILE *mfile, int voice);
unsigned long load_up_config(HWND hWnd, MEMORY_FILE *mfile, int voice);
unsigned long save_config(HWND hWnd, MEMORY_FILE *mfile, int voice);
unsigned long save_file(HWND hWnd, MEMORY_FILE *mfile, int voice);
//
//Module FILEBOX.C 
//
// STATIC functions for just this module
//		static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmfilebox(int *thresh, int *dur,int *min,int *max);
//
//Module GET_NUM.C 
//
// STATIC functions for just this module
//		static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmgetnum(char *title, int *x);
//
//Module ID.C 
//
int idstep(HWND hWnd);
int idnext(HWND hWnd);
int idnum(HWND hWnd);
int idpair(HWND hWnd);
int idfile(HWND hWnd);
//
//Module ID2BOX.C 
//
// STATIC functions for just this module
//		static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmid2box(int *num);
//
//Module IDBOX.C 
//
// STATIC functions for just this module
//		static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmidbox(int *num, char *name);
//
//Module LPC.C 
//
// STATIC functions for just this module
//	static void validate_dim();
//	static int getram();
//	static void freeram();
//	static void start_lpc();
//	static int get_frame();
//	static void startunlpc();
//	static void ucalc(void);
float *lpctran(char *start,
				char *end,
				float *transform,
				int *num_trans,
				TRAN_STRUCT *dims);

char *lpcuntran(float *tran, int num_trans,
			char *voice, TRAN_STRUCT *dims);
void tsolve();
// Here's hoping Microsoft has fixed that problem in the past 20 years... Removing the matherr function
//	int matherr (struct _exception *a);
//
//Module MAININ.C 
//
void CMUFileOpen( HWND hWnd );
void CMUColor( HWND hWnd );
void CMUFont( HWND hWnd );
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, unsigned int message, WPARAM wParam, LPARAM LParam);
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow);
//
//Module MFILE.C 
//
int mplay(HWND hWnd);
FILE *mfopen(char  name[255], char switches[4]);
int mfwrite(char *ptr, size_t one, size_t cnt, FILE *mfile);
int mfgetc(FILE *mfile);
int mfseek(FILE *mfile, long offset, int whence);
int mfclose(FILE *mfile);
//
//Module NULLFUNC.C 
//
int decoon(HWND hWnd);
//
//Module PLAY.C 
//
int playseg(HWND hWnd, SEGMENT *segptr);
int playrate(HWND hWnd);
int fake_header(char far *lptr, DWORD count);
//
//Module  RESIZEBO.C
//
// STATIC functions for just this module
//	static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmresizebox(int *scale);
//
//Module SEGBOX.C 
//
// STATIC functions for just this module
//	static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmsegbox(int *thresh, int *dur,int *min,int *max);
//
//Module SEGMENT.C 
//
int seglev(HWND hWnd);
int segdur(HWND hWnd);
int segmin(HWND hWnd);
int segmax(HWND hWnd);
int click(HWND hWnd, unsigned long x,unsigned long y);
int addseg(HWND hWnd, unsigned long x,unsigned long y);
int delseg(HWND hWnd, unsigned long x,unsigned long y);
int autosegment(HWND hwnd);
int clearseg();
//
//Module TRANBOX.C
//
// STATIC functions for just this module
//	static BOOL CALLBACK DlgBoxProc(HWND hDlg, unsigned int message,
//                           WPARAM wParam, LPARAM lParam);
void cmtranbox(int *size, int *interval, int *dim);
//
//Module TRANSFOR.C
//
int transize(HWND hWnd) ;
int traninterval(HWND hWnd);
int trandim(HWND hWnd);
int tranon(HWND hWnd);
int tranlpc(HWND hWnd);
int tranfft(HWND hWnd);
int trannopitch(HWND hWnd);
int tranhamm(HWND hWnd);
int tranrms(HWND hWnd);
int transelect(HWND hWnd);
int tranchange(HWND hWnd);
int tranall(HWND hWnd);
int tranclear(HWND hWnd);
int transeg(HWND hWnd, SEGMENT *segptr);
/* REMOVING UCL_SCL
//
//Module UCL_SCL.C 
//
int uclsclmain(HWND hWnd, int test, SEGMENT *seg);
int start(HWND hWnd);
int process_units(HWND hWnd);
int init_units_L();
int classify_vector();
int train_unit();
int display_stats(HWND hWnd);
int norm_vect();
int get_next_vect();
int screwy();
*/
//
//Module UNTRANSF.C 
//
int untrcalc(HWND hWnd);
int untrfile(HWND hWnd);
int untrsave(HWND hWnd);
int untrplay(HWND hWnd);
int untrseg(HWND hWnd, SEGMENT *segptr);
int decode(HWND hWnd);
//
//Module WAVID.C 
//
int identify_wav_resources(HWND hWnd);
