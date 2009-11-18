#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//only for COLOR_ constants
#include <curses.h>
#define COLOR_NONE -1

#define CLUE_RECORD_SEPARATOR 0
#define CLUE_WORD_SEPARATOR '\t' //tab == 9
#define CLUE_META_SEPARATOR '\b' //backspace == 8

#define CLUE_META_TYPE_GRAMMAR 0
#define CLUE_META_TYPE_CONTEXT 1
#define CLUE_META_TYPE_COUNTRY 2
#define CLUE_META_TYPE_REFERENCE 3 //clue >= 6.0

#define REALLOC_STEP 10 //string grow step size

#define BFS 64000 //size of reversed blocks

#define CLUELOCALE "ISO-8859-1"

#ifdef __cplusplus
class ClueRecord;

class ClueFile {
    private:
        FILE* fd;
        char* filename;
    public:
        ClueFile(const char* filename);
        ~ClueFile();
        //bool ClueFile::hasNextRecord();
        void seek(long offset);
        long tell();
        size_t length();
        ClueRecord* getPrevRecord();
        ClueRecord* getNextRecord();
        ClueRecord** binSearch(const char* searchword);
        ClueRecord** regexSearch(const char* searchword);
};

class ClueRecord {
    private:
        bool islocalized;
        char* word;
        char* translation;
        char* grammar;   // metafield 0
        char* context;   // metafield 1
        char* country;   // metafield 2
        char* reference; // metafield 3
        void init();
    public:
        ClueRecord();
        ClueRecord(char* cryptedrecord, uint recordlen);
        ~ClueRecord();
        const char* getWord() { return word; }
        const char* getTranslation() { return translation; }
        const char* getGrammar() { return grammar; }
        const char* getContext() { return context; }
        const char* getCountry() { return country; }
        const char* getReference() { return reference; }
        void localize();
        void printme();
        void printme(FILE* fd, bool color);
};
#endif

typedef unsigned int uint;
typedef unsigned char uchar;

struct cluerecord {
    char* word;
    char* translation;
    char* grammar;   // metafield 0
    char* context;   // metafield 1
    char* country;   // metafield 2
    char* reference; // metafield 3
};

#ifdef __cplusplus
extern "C" {
#endif

//defined in common.c
extern char* xbasename(const char* path);
extern void* xmalloc(size_t size);
extern void* xrealloc(void* ptr, size_t size);
extern char* xstrdup(const char* s);
extern char* xstrndup(const char* s, size_t n);
extern char* strjoin(const char** words, char separator);
extern char* strconv(const char* src, const char* from, const char* to);
extern char* strtolocal(const char* s);
extern char* strfromlocal(const char* s);
extern char* striconv(const char* src);
extern char* termcolorize(char* s, int color);

//defined in cluefile.cc
extern int recordCount(const char* filename);
extern void dumpClue(const char* filename);

//defined in libclue.c
extern char* mksortcompat(const char* s);
extern unsigned char decryptNibble (unsigned char al, unsigned char* mystic);
extern int cluecmp(const char* a, const char* b);

//defined in mapping.c
const char* filenameToLanguage(const char* filename);

//defined in install.c
void deCryptFile(const char* filename);

#ifdef __cplusplus
}
#endif
