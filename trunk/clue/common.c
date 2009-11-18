#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <langinfo.h>
#include <iconv.h>
#include <errno.h>
#include <locale.h>

#include "clue.h"


extern int errno;

// nicer version of basename (const, not ugly)
char* xbasename(const char* path) {
    char* ret;
    char* tmp = xstrdup(path);
    ret = xstrdup(basename(tmp));
    free(tmp);
    return ret;
}

//join strings
//(doesn't reuse memory of the strings)
char* strjoin(const char** words, char separator) {
    char* ret = 0; 
    int writect, len, i;
    writect = len = 0;

    if (words == 0) return 0;

    for (i = 0; words[i] != 0; i++) {
        len += strlen(words[i]) + 1;
        ret = (char*) xrealloc(ret, sizeof(char) * len);
        strcpy(&ret[writect], words[i]); 
        writect = len;
        ret[len - 1] = separator;
    }       
    ret[len - 1] = 0;
    return ret;
}

// malloc with error checking
void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) err(1, "malloc");
    return ptr;
}

// realloc with error checking
void* xrealloc(void* ptr, size_t size) {
    ptr = realloc(ptr, size);
    if (ptr == NULL) err(1, "realloc");
    return ptr;
}

// strdup with error checking
char* xstrdup(const char* s) {
    char* ptr = strdup(s);
    if (ptr == NULL) err(1, "strdup");
    return ptr;
}

// strndup with error checking
char* xstrndup(const char* s, size_t n) {
    char* ptr = strndup(s, n);
    if (ptr == NULL) err(1, "strndup");
    if (ptr[n] != 0) {
        puts("ERRRR");
    }
    return ptr;
}

char* strconv(const char* src, const char* from, const char* to) {
    iconv_t cd = (iconv_t) -1;
    char* dest;
    int destsize = REALLOC_STEP;
    char* srcptr; char* destptr;
    size_t srcleft, destleft;

    if (src == 0) return 0;
    
    cd = iconv_open(to, from);
    if (cd == (iconv_t) -1) err(1, "iconv_open");
    
    dest = (char*) xmalloc(destsize);
    srcptr = (char*) src; destptr = dest;
    srcleft = strlen(src) + 1; destleft = destsize;

    //printf("converting: %s\n", src);

    while (srcleft > 0) {
        size_t ret = iconv(cd, &srcptr, &srcleft, &destptr, &destleft);
        if (ret == (size_t) -1) {
            if (errno == E2BIG) {
                uint diff = destsize - destleft;
                destsize += REALLOC_STEP;
                destleft += REALLOC_STEP;
                dest = (char*) xrealloc(dest, destsize);
                destptr = dest + diff;
            } else {
                err(1, "iconv");
            }
        }
    }
    iconv_close(cd);

    //truncate to string size
    dest = (char*) xrealloc(dest, destsize - destleft); 
    return dest;
}

char* strtolocal(const char* s) {
    char* currentlocale = nl_langinfo(CODESET);
    if (!strcmp(currentlocale, "")) err(1, "nl_langinfo");
    return strconv(s, CLUELOCALE, currentlocale);
}

char* strfromlocal(const char* s) {
    char* currentlocale = nl_langinfo(CODESET);
    if (!strcmp(currentlocale, "")) err(1, "nl_langinfo");
    return strconv(s, currentlocale, CLUELOCALE);
}

// The decrypt char table is in iso88591 encoding.
// This function helps convert to local encoding.
char* striconv(const char* src) {
    static iconv_t cd = (iconv_t) -1;
    char* dest;
    int destsize = REALLOC_STEP;
    char* srcptr, * destptr;
    size_t srcleft, destleft;

    if (src == 0) return 0;
    
    if (cd == (iconv_t) -1) {
        char* currentlocale = nl_langinfo(CODESET);
        if (!strcmp(currentlocale, "")) err(1, "nl_langinfo");
        cd = iconv_open(currentlocale, CLUELOCALE);
        if (cd == (iconv_t) -1) err(1, "iconv_open");
        //XXX: sloppy: never using iconv_close
    }
    
    dest = (char*) xmalloc(destsize);
    srcptr = (char*) src; destptr = dest;
    srcleft = strlen(src) + 1; destleft = destsize;

    //printf("converting: %s\n", src);

    while (srcleft > 0) {
        size_t ret = iconv(cd, &srcptr, &srcleft, &destptr, &destleft);
        if (ret == (size_t) -1) {
            if (errno == E2BIG) {
                uint diff = destsize - destleft;
                destsize += REALLOC_STEP;
                destleft += REALLOC_STEP;
                dest = (char*) xrealloc(dest, destsize);
                destptr = dest + diff;
            } else {
                fprintf(stderr, 
"\nEncoding text failed. Your locale encoding must be a superset of ISO-8859-1.\n");
                err(1, "iconv");
            }
        }
    }
    //truncate to string size
    dest = (char*) xrealloc(dest, destsize - destleft); 
    return dest;
}

char* termcolorize(char* s, int colornum) {
    char* ret; size_t len;
    if (colornum != COLOR_NONE) {
        len = 50; //more than enough for the color codes
        len += strlen(s);
        ret = (char*) xmalloc(len);
        snprintf(ret, len, "\033[1;3%um%s\033[0;39m", colornum, s);
        ret = (char*) xrealloc(ret, strlen(ret) + 1);
    } else {
        ret = xstrdup(s);
    }
    return ret;
}

