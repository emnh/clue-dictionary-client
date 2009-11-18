#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "clue.h"

using namespace std;

void ClueRecord::init() {
    word = 0;
    translation = 0;
    grammar = 0;
    context = 0;
    country = 0;
    reference = 0;
    islocalized = false;
}

ClueRecord::ClueRecord() {
    init();
}

ClueRecord::ClueRecord(char* cryptedrecord, uint recordlen) {
    //mystic[0] is never used
    unsigned char mystic[5] = {0,0,0,0,1};
    unsigned char al, chr;
    size_t cwsize = REALLOC_STEP;
    char* cword = (char*) xmalloc(cwsize);
    uint wordpos = 0, i, metacount = 0;
    char* tmp;

    init();
    
    //process record nibble by nibble (4-bit)
    for (i = 0; i < recordlen * 2; i++) {
        chr = (uchar) cryptedrecord[i / 2];
        if (i % 2 == 0) {
            //al = upper four bits (xxxxyyyy) -> (0000xxxx)
            al = chr >> 4;
        } else {
            //al = lower four bits (xxxxyyyy) -> (0000yyyy)
            al = chr & 0xF;
        }
        
        //decrypt character
        chr = decryptNibble(al, mystic);
        switch(chr) {
            case 0:
                //No output char yet, only state changes in the mystic array
                break;
            case CLUE_META_SEPARATOR:
                tmp = xstrndup(cword, wordpos);
                switch (metacount) {
                    case CLUE_META_TYPE_GRAMMAR:
                        grammar = tmp;
                        break;
                    case CLUE_META_TYPE_CONTEXT:
                        context = tmp;
                        break;
                    case CLUE_META_TYPE_COUNTRY:
                        country = tmp;
                        break;
                    case CLUE_META_TYPE_REFERENCE:
                        reference = tmp;
                        break;
                    default:
                        fprintf(stderr, "error: unknown metainfo type: %u\n",
                                metacount);
                        fprintf(stderr, "the metainfo string was: %s\n", tmp);
                        break;
                }
                metacount++;
                wordpos = 0;
                break;
            case CLUE_WORD_SEPARATOR:
                word = xstrndup(cword, wordpos);
                wordpos = 0;
                break;
            default:
                //fprintf(stderr, "read char: %c\n", chr);
                if (wordpos >= cwsize) {
                    cwsize += REALLOC_STEP;
                    cword = (char*) xrealloc(cword, cwsize);
                }
                cword[wordpos] = (char) chr;
                wordpos++;
                break;
        }
    }
    translation = xstrndup(cword, wordpos);
    if (cword) free(cword);
}

ClueRecord::~ClueRecord() {
    free(this->word);
    free(this->translation);
    free(this->grammar);
    free(this->country);
    free(this->context);
    free(this->reference);
}

void ClueRecord::printme() {
    this->printme(stdout, true);
}

void ClueRecord::localize() {
    char* tmp;

    if (islocalized) return;

    islocalized = true;

    tmp = this->word;
    this->word = strtolocal(this->word);
    free(tmp);

    tmp = this->translation;
    this->translation = strtolocal(this->translation);
    free(tmp);

    tmp = this->grammar;
    this->grammar = strtolocal(this->grammar);
    free(tmp);

    tmp = this->context;
    this->context = strtolocal(this->context);
    free(tmp);

    tmp = this->country;
    this->country = strtolocal(this->country);
    free(tmp);

    tmp = this->reference;
    this->reference = strtolocal(this->reference);
    free(tmp);
}

void ClueRecord::printme(FILE* fd, bool color) {
    char* tmp; 
    localize();
    strlen(word);
    int word_color = COLOR_BLUE;
    int reference_color = COLOR_MAGENTA;
    int grammar_color = COLOR_RED;
    int country_color = COLOR_CYAN;
    int context_color = COLOR_WHITE;
    if (!color) {
        word_color = COLOR_NONE;
        reference_color = COLOR_NONE;
        grammar_color = COLOR_NONE;
        country_color = COLOR_NONE;
        context_color = COLOR_NONE;
    }
    if (word && word[0] != 0) {
        tmp = termcolorize(word, word_color);
        fprintf(fd, "%s:", tmp);
        free(tmp);
    } else {
        printf("Error: word is not set!");
        exit(1);
    }
    if (reference && reference[0] != 0) {
        tmp = termcolorize(reference, reference_color);
        fprintf(fd, " >%s<", tmp);
        free(tmp);
    }
    if (grammar && grammar[0] != 0) {
        tmp = termcolorize(grammar, grammar_color);
        fprintf(fd, " (%s.)", tmp);
        free(tmp);
    }
    if (country && country[0] != 0) {
        tmp = termcolorize(country, country_color);
        fprintf(fd, " (%s)", tmp);
        free(tmp);
    }
    if (context && context[0] != 0) {
        tmp = termcolorize(context, context_color);
        fprintf(fd, " [%s]", tmp);
        free(tmp);
    }
    fprintf(fd, " %s", translation);
    fputs("\n", fd);
}

int newrecordCount(const char* filename) {
    char* cryptedrecord = 0;
    size_t n = 0;
    FILE* fd = fopen(filename, "r");
    if (fd == NULL) err(1, "%s", filename);
    int count = 0;
    while (1) {
        ssize_t readct = getdelim(&cryptedrecord, &n,
                         CLUE_RECORD_SEPARATOR, fd);
        if (readct == -1) break;
        count++;
    }
    if (cryptedrecord) free(cryptedrecord);
    fclose(fd);
    return count;
}

int recordCount(const char* filename) {
    ClueFile cf(filename);
    ClueRecord* rec;
    int count = 0;
    
    while ((rec = cf.getNextRecord())) {
        delete rec;
        count++;
    }

    return count;
}

extern "C" void dumpClue(const char* filename) {
    ClueFile cf(filename);
    ClueRecord* rec;
    
    while ((rec = cf.getNextRecord())) {
        rec->printme();
        delete rec;
    }
}
