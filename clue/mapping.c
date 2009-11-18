#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "clue.h"

char* mapping[] = {
"clabfr.dat", "French Abbreviations",
"clabno.dat", "Norwegian Abbreviations",
"clabsv.dat", "Swedish Abbreviations",
"clabuk.dat", "English Abbreviations",
"cldenomx.dat", "German -> Norwegian",
"clesukmx.dat", "Spanish -> English",
"clfrukmx.dat", "French -> English",
"clsvukmx.dat", "Swedish -> English",
"clnodemx.dat", "Norwegian -> German",
"clnono.dat", "Norwegian -> Norwegian",
"clnonome.dat", "Norwegian Medical",
"clnoukmx.dat", "Norwegian -> English",
"clukesmx.dat", "English -> Spanish",
"clukfrmx.dat", "English -> French",
"cluknomx.dat", "English -> Norwegian",
"cluksvmx.dat", "English -> Swedish",
"clukuk.dat", "English -> English",
0
};

const char* filenameToLanguage(const char* path) {
    int i; char* bname; char* language = NULL;
    bname = xbasename(path);
    for (i = 0; mapping[i] != 0; i += 2) {
        if (!strcasecmp(bname, mapping[i])) {
            language = mapping[i + 1];
            break;
        }
    }
    free(bname);
    if (language == NULL) {
        return "unknown language";
    } else {
        return mapping[i + 1];
    }
}
