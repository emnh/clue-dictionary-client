#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

#include "clue.h"

using namespace std;

ClueFile::ClueFile(const char* filename) {
    fd = fopen(filename, "r");
    if (fd == NULL) err(1, "%s", filename);
    this->filename = xstrdup(filename);
}

long ClueFile::tell() {
    long pos = ftell(fd);
    if (pos == -1) err(1, "ftell");
    return pos;
}

void ClueFile::seek(long offset) {
    if (fseek(fd, offset, SEEK_SET) != 0)
        err(1, "fseek");
}

size_t ClueFile::length() {
    struct stat stat_buf;
    if (fstat(fileno(fd), &stat_buf) == -1) err(1, "fstat");
    return stat_buf.st_size;
}

/*void ClueFile::fwseekRecord() {
}*/

ClueRecord* ClueFile::getPrevRecord() {
    ClueRecord* rec;

    long recstart = -1;

    long curpos = ftell(fd);
    if (curpos == -1) err(1, "ftell");
    if (curpos == 0) return NULL;

    while (true) {
        curpos = ftell(fd);
        if (curpos == -1) err(1, "ftell");
        curpos = (curpos < 2) ? 0 : curpos - 2;
        
        if (fseek(fd, curpos, SEEK_SET) != 0)
            err(1, "fseek");
        
        if (curpos == 0) { 
            recstart = 0; 
            break;
        }
        
        char c = getc(fd);
        if (c == 0) {
            recstart = ftell(fd);
            if (recstart == -1) err(1, "ftell");
            break;
        }
    }
    if (recstart >= 0) {
        rec = getNextRecord();
        if (fseek(fd, recstart, SEEK_SET) != 0)
            err(1, "fseek");
    } else {
        rec = NULL;
    }
    return rec;
}

ClueRecord* ClueFile::getNextRecord() {
    char* cryptedrecord = 0; size_t n = 0;
    ssize_t readct = 1;
    ClueRecord* rec;

    //readct == 1 means empty record
    while (readct == 1) {
        readct = getdelim(&cryptedrecord, &n,
                     CLUE_RECORD_SEPARATOR, fd);
    }

    if (readct == -1) 
        rec = NULL;
    else {
        rec = new ClueRecord(cryptedrecord, readct);
        if (cryptedrecord) free(cryptedrecord);
    }
    return rec;
}

ClueFile::~ClueFile() {
    fclose(fd);
    free(filename);
}

ClueRecord** ClueFile::binSearch(const char* searchword) {
    ClueRecord* rec = NULL;
    ClueRecord** matches = NULL; 
    size_t lower = 0;
    struct stat stat_buf;
    if (fstat(fileno(fd), &stat_buf) == -1) err(1, "fstat");
    size_t upper = stat_buf.st_size;
    int cmp = 0;
    char* lsearchword = NULL;

    lsearchword = strfromlocal(searchword);
    while (1) {
        int guess = lower + (upper - lower) / 2; 
        seek(guess);
        rec = getPrevRecord();
        cmp = cluecmp(rec->getWord(), lsearchword);
        if (cmp == 0) {
            int matchct = 0;
            int foundpos = tell();
            while (rec != NULL && cluecmp(lsearchword, rec->getWord()) == 0) {
                foundpos = tell();
                rec = getPrevRecord();
            }
            seek(foundpos);
            rec = getNextRecord();
            while (rec != NULL && cluecmp(lsearchword, rec->getWord()) == 0) {
                matches = (ClueRecord**) xrealloc(matches, sizeof(ClueRecord*) * (matchct + 2));
                matches[matchct++] = rec;
                rec = getNextRecord();
            }
            matches[matchct] = NULL;
            break;
        }
        delete rec;
        if (cmp < 0) {
            lower = guess;
        } else {
            upper = guess;
        }

        if (abs(lower - upper) <= 1)
            break;
    }
    free(lsearchword);
    return matches;
}

ClueRecord** ClueFile::regexSearch(const char* searchexpr) {
    ClueRecord* rec = NULL;
    ClueRecord** matches = NULL; 
    regex_t* preg = NULL;
    int cmp = 0; char* lsearchexpr = NULL;
    int matchct = 0;
    lsearchexpr = strfromlocal(searchexpr);
    preg = (regex_t*) malloc(sizeof(regex_t));
    if (regcomp(preg, lsearchexpr, REG_NOSUB)) err(1, "regcomp");
    seek(0);
    while (1) {
        rec = getNextRecord();
        if (rec == NULL) break;
        if (regexec(preg, rec->getWord(), 0, 0, 0) == 0) {
            matches = (ClueRecord**) xrealloc(matches, sizeof(ClueRecord*) * (matchct + 2));
            matches[matchct++] = rec;
        } else {
            delete rec;
        }
    }
    if (matches != NULL) matches[matchct] = NULL;
    free(lsearchexpr);
    regfree(preg);
    return matches;
}
