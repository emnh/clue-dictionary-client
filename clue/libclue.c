/** Encoding: ISO-8859-1 **/

//because of getdelim
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <ctype.h>
#include <locale.h>

#include "clue.h"
#include "accents.h"

/*
Clue file format in decoded form:
(<lookup word><tab>(<metainfo><backspace>)*<translation text><null>)*

content types:
<lookup word>: the word we want to translate
<metainfo>: grammatical info such as noun, adjective
            or classification, such as historical, music
<metainfo0> grammatical
<metainfo1> contextual
<metainfo2> language/country
<metainfo3> reference (to related lookup word) (clue >= 6.0)
<translation text>: the translation of the lookup word

separators:
<tab>: '\t' == 9: separates the lookup word from the lookup result
<backspace>: '\b' == 8: separates meta-information about the lookup result
<null>: separates each lookup record
*/

/** NOTE: You can not convert the character table to multibyte encodings
    without making the decompression code multibyte aware.
**/

//this is not 0-255, but close
const char* alphatable = 
"\000\001\002\003\004\005\006\a\b\t\n\v\f\r\016\017\020\021\022\023\024\025\
\026\027\030\031\032\e\034\035\036\037 !\"#$%&'()*+,-./0123456789:;<=>?@AB\
CDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\177Çüéâäàåçê\
ëèïîìÄÅÉæÆôöòûùÿÖÜø£Ø×¿áíóúñÑªº¿®¬½¼¡«»¿¿¿¿¿ÁÂÀ©¿¿¿¿¢¥\234¿¿¿¿¿¿ãÃ¿¿¿¿¿¿¿¤\
ðÐÊËÈ¿ÍÎÏ¿¿¿¿¦Ì¿ÓßÔÒõÕµþÞÚÛÙýÝ¯´·±¿¾¶§÷¸°¨¿¹³²¿¿\001";

//using \000 instead of \0 so they won't merge into following ascii numbers
//these are really five tables with 16 characters each
//denoted tables16-0, tables16-1 etc. later on
const char* tables16 = 
"\t eartnsli\b,   \000\
bcdfghjkmpquvwx\000\
yzøå-.Aæ()1*!SU\000\
59BWDE'0CTé:è/2\000\
3KG46LHOFIJ8Po \001\000";

unsigned char decryptNibble (unsigned char al, unsigned char* mystic) {
    unsigned char uncommonchar = mystic[1];
    unsigned char veryrarechar = mystic[2];
    unsigned char savedchardata = mystic[3];
    unsigned char is_reading_lookup_word = mystic[4];
    
    //al is a 4-bit variable with range 0-15
    
    int retval = 0;

    /*
       Remember that a byte here may be last 4 bits of one byte
       and 4 first bits of the next byte as they are laid out in a file
    
       Frequently occuring characters:
       A nibble of 0-11:
       0 is word separator or padding
       1-11 is a character in tables16-0

       Uncommonly occuring characters:
       A nibble of 12-15:
       Next nibble is an index in one of
       tables16-1,2,3,4 as determined by
       previous nibble val - 12 + 1
       
       Very rarely occuring characters:
       A byte of 0xFF:
       Next byte will be an index in alphatable
    */

    if (uncommonchar == 0) {
        //al in range 0-11
        if (al < 0xC) {
            //al == 0 is a terminator for lookup word
            //or if not reading lookup word, it is padding
            if (al == 0) {
                if (is_reading_lookup_word != 0) {
                    is_reading_lookup_word = 0;
                    retval = CLUE_WORD_SEPARATOR;
                }
            } else {
                //Frequent character:
                //Return one of the first 12 chars in tables16-0
                //all of which are frequently occuring characters
                retval = tables16[al];
            }
        //al in range 12-15
        } else {
            uncommonchar = 1;
            veryrarechar = 0;
            savedchardata = al;
            //al is an index into tables16 1,2,3,4
            //each of 16 characters.
            //Or if next nibble is 0xF, then
            //next byte is char in alphatable.
        }
    } else {
        if (veryrarechar == 0) {
            if (savedchardata == 0xF && al == 0xF) {
                veryrarechar = 1;
                savedchardata = 0;
            } else {
                uncommonchar = 0;
                //Using savedchardata as index in tables16-1,2,3,4
                //and al as index in the chosen tables16
                retval = tables16[(savedchardata - 0xC + 1) * 16 + al - 1];
            }
        } else {
            //Get character from the big table,
            //using a whole byte as index.
            //Used for infrequent characters.
            if (savedchardata == 0) {
                //Put the first nibble in the higher order bits
                savedchardata = al << 4;
            } else {
                //Put the second nibble in the lower order bits 
                al |= savedchardata;
                veryrarechar = 0;
                uncommonchar = 0;
                retval = alphatable[al]; //alphatable
            }
        }
    }
    mystic[1] = uncommonchar;
    mystic[2] = veryrarechar;
    mystic[3] = savedchardata;
    mystic[4] = is_reading_lookup_word;
    return retval;
}

/*not entirely perfect
you can't use it for sorting
it would be slightly wrong
but binsearch works fine */
/** Should really replace this crap with something better soon.
 * **/
char* mksortcompat(const char* s) {
    int len, i, j = 0, growct = 0;
    char* ret; char* origlocale;
    
    //translate or delete char (-> 0)
    const char trans[] = {
        '-', 0,
        '.', 0,
        '!', 0,
        '\'', 0,
        '?', 0,
        0, 0
    };

    const char* tables[] = {
        //trcase, accents, trans, 0
        accents, trans, 0
    };

    //make C functions locale aware
    origlocale = setlocale(LC_ALL, "nn_NO.iso88591");
    //free(origlocale);

    len = strlen(s);

    ret = (char*) xmalloc(len + 1);

    for (i = 0; i < len; i++) {
        int k, ti;
        const char* table;
        char c = tolower(s[i]);

        //process translation tables
        for (ti = 0; tables[ti] != 0; ti++) {
            table = tables[ti];
            for (k = 0; table[k] != 0; k += 2) {
                if (c == table[k]) {
                    c = table[k + 1];
                    break;
                }
            }
        }
        
        //double s
        if (c == 'ß') {
            ret = (char*) xrealloc(ret, len + ++growct +  1);
            ret[j++] = 's';
            c = 's';
        };

        //truncate string when '/' is found
        if (c == '/') break;

        //truncate string when '(' is found
        if (c == '(') break;

        if (c != 0)
            ret[j++] = c;
    }

    //strip trailing space resulting from " -"
    //so that example: "multi-axle" == "multi-axle -"
    if (j > 0 && ret[j - 1] == ' ') j--;
    ret[j] = 0;
    ret = (char*) xrealloc(ret, j + 1);
    
    //restore locale
    setlocale(LC_ALL, "");
    
    return ret;
}

int cluecmp(const char* a, const char* b) {
    int ret;
    char* ac, * bc;
    ac = mksortcompat(a);
    bc = mksortcompat(b);
    //setlocale(LC_ALL, "nn_NO.iso88591");
    ret = strcmp(ac, bc);
    //setlocale(LC_ALL, "");
    free(ac);
    free(bc);
    return ret;
}
