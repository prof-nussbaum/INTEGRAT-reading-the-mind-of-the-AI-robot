#include <stdio.h>
#include <mem.h>
#include <string.h>
#include <memory.h>
#include <windows.h>
#include <mmsystem.h>


#define HALFMEG (unsigned long)(1024.0 * 1024.0 * 0.5)

// this structure contains a "memory File" of 1/2 meg blocks

typedef struct memory_file {
    FILE *count;
    char lpszFilename[255];
    char switches[4];
    int fnOpenMode;
    HFILE hpFile;
	char *cur_pos;
    unsigned long size;
	char *data;
}MEMORY_FILE;

MEMORY_FILE imem;
MEMORY_FILE omem;
FILE *counter = 1;

mplay(HWND hWnd)
{
    unsigned char *parse1 = "RIFFxxxxWAVEfmt xxxxxxxxxxxxxxxxxxxxdataxxxz";
	unsigned char *parse2 = imem.data;
    unsigned long size;
    int x;

    if (!parse2) return(0);
    while (*parse1 != 'z') *(parse2++) = *(parse1++);
    size = imem.size;
    parse2 = imem.data + 4;
    size += 36;
    for (x = 0; x < 4; x++) {
        *(parse2++) = (size & 0x000000FF);
        size >>= 8;
    }
    parse2 += 8;
    *(parse2++) = 0x10;
    *(parse2++) = 0x00;
    *(parse2++) = 0x00;
    *(parse2++) = 0x00;
    *(parse2++) = 0x10;
    *(parse2++) = 0x00;
    *(parse2++) = 0x10;
    *(parse2++) = 0x00;
    *(parse2++) = 0x11;
    *(parse2++) = 0x2b;
    *(parse2++) = 0x00;
    *(parse2++) = 0x00;
    *(parse2++) = 0x11;
    *(parse2++) = 0x2b;
    *(parse2++) = 0x00;
    *(parse2++) = 0x00;
    *(parse2++) = 0x01;
    *(parse2++) = 0x00;
    *(parse2++) = 0x08;
    *(parse2++) = 0x00;
    parse2 += 4;
    size = imem.size;
    size -= 1;
    for (x = 0; x < 4; x++) {
        *(parse2++) = (size & 0x000000FF);
        size >>= 8;
    }
    sndPlaySound(imem.data, SND_ASYNC | SND_MEMORY);

}

FILE *mfopen(char  name[255], char switches[4])
{
    MEMORY_FILE *file;
    unsigned int size;
    OFSTRUCT ofFile[1];


    if ((switches[0] == 'r') || (switches[0] == 'R')) {
        file = &imem;
        file->fnOpenMode = OF_CANCEL | OF_PROMPT | OF_READ;
    }
    else if ((switches[0] == 'w') || (switches[0] == 'W')) {
        file = &omem;
        file->fnOpenMode = OF_CANCEL | OF_CREATE | OF_WRITE;
    }
    else
        return(0);
    lstrcpy(file->switches, switches);
    lstrcpy(file->lpszFilename, (LPCSTR)name);
    file->hpFile = OpenFile(file->lpszFilename, ofFile, file->fnOpenMode);
    if (file->hpFile == HFILE_ERROR)
            return(0);

	if (!(file->data = (char*) calloc(1, HALFMEG)))
            return(0);
    if ((switches[0] == 'r') || (switches[0] == 'R')) {
        for (file->size = 0, file->cur_pos = file->data;
                file->size < HALFMEG;) {
            size = _lread(file->hpFile, file->cur_pos, (unsigned int)(1024.0 * .25));
            file->size += size;
            file->cur_pos += size;
            if ((size < (unsigned int)(1024.0 * .25)) || (!size))
                break;
        }
        if (size == HFILE_ERROR)
                return(0);
    }
    else
        file->size = 0;
    file->cur_pos = file->data;

    return(file->count = counter++);
}
int mfwrite(char *ptr, size_t one, size_t cnt, FILE *mfile)
{
    unsigned long result;

    if (omem.size + (one * cnt) > HALFMEG) {
        for (omem.cur_pos = omem.data, result = 0;
                result <= omem.size - (1024.0 * .25);) {
            result += _lwrite(omem.hpFile, omem.cur_pos, (unsigned int)(1024.0 * .25));
            omem.cur_pos = &omem.data[result];
        }
        if (result != omem.size)
            result += _lwrite(omem.hpFile, omem.cur_pos,
                        (unsigned int)(omem.size % (unsigned long)(1024.0 * .25)));
        omem.size = 0;
        omem.cur_pos = omem.data;
    }

    memcpy(omem.cur_pos, ptr, one * cnt);
    omem.cur_pos += one * cnt;

    if ((char *)omem.cur_pos > (char *)&omem.data[omem.size]) {
        omem.size = (omem.cur_pos - omem.data);
    } 
    return (result);
}
int mfgetc(FILE *mfile)
{
    unsigned long size;
    int result;

    if ((imem.size >= HALFMEG) &&
        (imem.cur_pos > &imem.data[HALFMEG])) {
        for (imem.cur_pos = imem.data, imem.size = 0;
            imem.size < HALFMEG;) {
            size = _lread(imem.hpFile, imem.cur_pos, (unsigned int)(1024.0 * .25));
            imem.size += size;
            imem.cur_pos = &imem.data[imem.size];
            if ((size < (unsigned int)(1024.0 * .25)) || (!size))
                break;
        }
        imem.cur_pos = imem.data;
    }
    result = *(imem.cur_pos++);
    if (imem.cur_pos > &imem.data[imem.size]) {
        result = EOF;
        imem.cur_pos--;
    }

    return(result);
}
int mfseek(FILE *mfile, long offset, int whence)
{
    if ((whence == 0) && (offset == 0)) {
        if (mfile == imem.count) {
            imem.cur_pos = imem.data;
        }
        else {
            omem.cur_pos = omem.data;
        }

    }
}
int mfclose(FILE *mfile)
{
    unsigned long result;

    if (mfile == imem.count) {

		free(imem.data);
        _lclose(imem.hpFile);
    }
    else {
        for (omem.cur_pos = omem.data, result = 0;
                result <= omem.size - (1024.0 * .25);) {
            result += _lwrite(omem.hpFile, omem.cur_pos, (unsigned int)(1024.0 * .25));
            omem.cur_pos = &omem.data[result];
        }
        if (result != omem.size)
            result += _lwrite(omem.hpFile, omem.cur_pos,
                        (unsigned int)(omem.size % (unsigned long)(1024.0 * .25)));

        free(omem.data);
        _lclose(omem.hpFile);
    }
    return(1);
}
