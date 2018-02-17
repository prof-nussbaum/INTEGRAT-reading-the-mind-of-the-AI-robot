// INTEGRAT STRUCTURES
//------------------STRUCTURES------------------
typedef struct memory_file {
    FILE *count;
    char lpszFilename[255];
    char switches[4];
    int fnOpenMode;
    HFILE hpFile;
//    unsigned char huge *cur_pos;
    char *cur_pos;
    unsigned long size;
//	char huge *data;
	char *data;
}MEMORY_FILE;

typedef struct file_struct {
	MEMORY_FILE open;
	MEMORY_FILE con;
	MEMORY_FILE tra;
	MEMORY_FILE cod;
	char		play;
	char		rec;
} FILE_STRUCT;

typedef struct segment {
//	char huge *start, huge *end;
	char *start, *end;
	unsigned char class1, class2;
	int num_trans;
//	float huge *tran_start;
	float *tran_start;
} SEGMENT;

typedef struct  {
	int 	lev;
	int		dur;
	int		min;
	int 	max;
	SEGMENT segments[128];
} SEG_STRUCT;

typedef struct id {
	unsigned char number;
	char name[10];
    unsigned long int examples;
} ID;

typedef struct id_struct {
	char			next;
	int				num;
	char			pair;
	ID				ids[129];
	unsigned long int	count;
} ID_STRUCT;

typedef struct tran_struct {
	char		on;
	char		lpc;
	char		fft;
	char		rms;
	int			size;
	int		interval;
	int			dim;
	char	nopitch;
	char	hamm;
}TRAN_STRUCT;

typedef struct untr_struct {
	char		calc;
	char		file[255];
	char		save;
	char		play;
} UNTR_STRUCT;

typedef struct code_struct {
	char		on;
	MEMORY_FILE file;
	char	ucl;
	char	autoassc;
	char	scl;
	char	back;
	unsigned long int	iter;
} CODE_STRUCT;

typedef struct deco_struct {
	char	on;
} DECO_STRUCT;

typedef struct config_struct {
	FILE_STRUCT	file;
	SEG_STRUCT	seg;
	ID_STRUCT	id;
	TRAN_STRUCT	tran;
	UNTR_STRUCT	untr;
	CODE_STRUCT	code;
	DECO_STRUCT	deco;
	char end;
}CONFIG_STRUCT;

