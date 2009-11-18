//for getdelim
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <popt.h>
#include <locale.h>
#include <dirent.h>
#include <readline/history.h>
#include <readline/readline.h>
#include "clue.h"

//#define DEBUG

void sortTest (const char* filename) {
    int tc = 0;
    int bads = 0;
    char* oldword = NULL;
    ClueFile cf(filename);
    ClueRecord* rec;
    while((rec = cf.getNextRecord()) != NULL) {
        char* word = xstrdup(rec->getWord());
        tc++;
        if (oldword != NULL) {
            if (cluecmp(oldword, word) > 0) {
                bads++;
                char* a = strtolocal(oldword);
                char* b = strtolocal(word);
                char* sword = mksortcompat(word);
                char* soldword = mksortcompat(oldword);
                char* c = strtolocal(soldword);
                char* d = strtolocal(sword);
                free(soldword);
                free(sword);
                printf("\"%s\" is greater than \"%s\" (%s, %s)\n", a, b, c, d);
                free(a);
                free(b);
                free(c);
                free(d);
            }
        }
        free(oldword);
        oldword = word;
        delete rec;
    }
    free(oldword);
    printf("%u out of %u failed; accuracy %f\n", bads, tc, 1.0 - (float) bads / (float) tc);
    
}

int curlang = 0;
int filect = 0;
const char** filenames = NULL;

char* update_prompt (const char* filename) {
    static char prompt[500];
    const char* language = filenameToLanguage(filename);
    snprintf(prompt, sizeof(prompt), "clue %s> ", language);
    return prompt;
}

// For interactive mode. Bound to TAB.
int change_language(int a, int b) {
    curlang = (curlang + 1) % filect;
    char* prompt = update_prompt(filenames[curlang]);
    rl_set_prompt(prompt);
    return 0;
}

int file_filter(const struct dirent* direntry) {
    struct stat buf; int statret = 0;
    const char* filename = direntry->d_name;
    statret = (stat(filename, &buf));
    if (statret) err(1, "stat");
    if (S_ISDIR(buf.st_mode)) {
        return false;
    }
    const char* ext = ".DAT";
    const char* last = filename + strlen(filename) - strlen(ext);
    if (strcasecmp(last, ext) == 0) return true;
    return false;
}

// Find files in directory and update filenames
void find_files(const char* directory) {
    const char* joinstrings[3] = {0};
    const char* filename = NULL;
    static struct dirent** dirfiles = NULL;
    if (chdir(directory) != 0) {
        err(1, "%s", directory);
    };
    int n = scandir(".", &dirfiles, &file_filter, alphasort);
    if (n == -1) {
        err(1, "scandir: %s", directory);
    }
    joinstrings[0] = directory;
    while (n--) {
        joinstrings[1] = dirfiles[n]->d_name;
        filename = strjoin(joinstrings, '/');
        filenames = (const char**) xrealloc(filenames, sizeof(char*) * (filect + 1));
        filenames[filect] = filename;
        filect++;
        free(dirfiles[n]);
    }
    free(dirfiles);
}

char* rl_interactive_completion_function(const char* text, int state) {
    static ClueFile* cf = NULL;
    static ClueRecord** recs = NULL;
    int i = 0;
    if (state == 0) {
        if (cf) {
            delete cf;
            while(recs[i]) {
                delete recs[i++];
            }
            delete recs;
        }
        cf = new ClueFile(filenames[curlang]);
        char newtext[500];
        snprintf(newtext, sizeof(newtext), "^%s", text);
        recs = cf->regexSearch(newtext);
    }
    if (recs == NULL || recs[state] == NULL)
        return NULL;
    else {
        char* word = xstrdup(recs[state]->getWord());
        return word;
    }
}

int main (int argc, char* argv[]) {
    poptContext optCon;
    int i;
    int dump, count, install, sorttest;
    char c; 
    const char** words;
    bool regexsearch = false;
    bool interactive = false;
    bool verbose = false;
    bool nocolor = false;

    //enable localization
    setlocale(LC_ALL, "");
    
    dump = count = install = sorttest = 0;
    
    // 5'th arg: the value return from poptGetNextOpt later on
    struct poptOption optionsTable[] = {
        { "count-words", 'c', POPT_ARG_NONE, &count, 0,
          "count the number of lookup words", 0
        },
        { "filename", 'f', POPT_ARG_STRING, 0, 'f',
          "dictionary filenames", "filename"
        },
        { "dict-dir", 'D', POPT_ARG_STRING, 0, 'D',
          "directory to scan for dictionary files", "directory"
        },
        { "interactive", 'i', POPT_ARG_NONE, &interactive, 0,
          "interactive mode", 0
        },
        { "sort-test", 's', POPT_ARG_NONE, &sorttest, 0,
          "test the accuracy of the comparison operator", 0
        },
        { "regex", 'r', POPT_ARG_NONE, &regexsearch, 0,
          "regular expression search"
        },
        { "dump", 'd', POPT_ARG_NONE, &dump, 0,
          "dump dictionary to stdout", 0
        },
        { "verbose", 'v', POPT_ARG_NONE, &verbose, 0,
          "be more verbose", 0
        },
        { "no-color", 0, POPT_ARG_NONE, &nocolor, 0,
          "disable ANSI-coloring of output", 0
        },
        { "install", 0, POPT_ARG_NONE, &install, 0,
          "decrypt dictionary for installation", 0
        },
        POPT_AUTOHELP
        POPT_TABLEEND
    };

    optCon = poptGetContext("clue", argc, (const char**) argv, optionsTable, 0);
    
    // check for help/usage and restart if not
    //poptGetNextOpt(optCon);
    //poptResetContext(optCon);

    poptSetOtherOptionHelp(optCon, "<lookup word>");

    if (argc < 2) {
        poptPrintHelp(optCon, stderr, 0);
        exit(1);
    }

    //option parsing
    while ((c = poptGetNextOpt(optCon)) >= 0) {
        char* filename = NULL;
        const char* directory = NULL;
        switch (c) {
            case 'D':
                directory = poptGetOptArg(optCon);
                find_files(directory);
                break;
            case 'f':
                filename = xstrdup(poptGetOptArg(optCon));
                filenames = (const char**) xrealloc(filenames, sizeof(char*) * (filect + 1));
                filenames[filect] = filename;
                filect++;
                break;
            case 'r':
                filect = 0;
                break;
            default:
                fprintf(stderr, "option parsing: this should never happen\n");
                break;
        }
    }

    if (c < -1) {
        fprintf(stderr, "%s: %s\n",
                poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
                poptStrerror(c));
        exit(1);
    }

    if (filect == 0) find_files(DEFAULT_DATA_DIR);

    if (filect == 0) {
        fprintf(stderr, "error: no dictionary files found/specified\n");
        exit(1);
    }

    words = poptGetArgs(optCon);

    //action parsing
    if (interactive) {
        rl_bind_key('\t', change_language);
        //rl_completion_entry_function = &rl_interactive_completion_function;
        using_history();
        while (true) {
            char* prompt = update_prompt(filenames[curlang]);
            char* line = readline(prompt);
            char* word = line;

            if (line == NULL) break;
            if (line[0] != 0) add_history(line);

            ClueRecord** recs;
            ClueFile cf(filenames[curlang]);

            if (regexsearch) {
                recs = cf.regexSearch(word);
            } else {
                recs = cf.binSearch(word);
            }

            if (recs != NULL) {
                //printf("Search results for %s:\n", language);
                for (int i = 0; recs[i] != NULL; i++) {
                    ClueRecord* rec = recs[i];
                    rec->printme(stdout, !nocolor);
                    delete rec;
                }
                free(recs);
            } 
            free(line);
        }
    } else {
        for (i = 0; i < filect; i++) {
            const char* filename = filenames[i];
            if (verbose) {
                printf("Processing file %s\n", filename);
            }
            if (count) printf("%u\n", recordCount(filename));
            if (dump) dumpClue(filename);
            if (install) deCryptFile(filename);
            if (sorttest) sortTest(filename);
        
            if (words != NULL) {
                char* word = strjoin(words, ' ');
                const char* language = filenameToLanguage(filename);
                ClueRecord** recs;
                ClueFile cf(filename);
                if (regexsearch) {
                    recs = cf.regexSearch(word);
                } else {
                    recs = cf.binSearch(word);
                }
                free(word);
                if (recs != NULL) {
                    printf("%s:\n", language);
                    for (int i = 0; recs[i] != NULL; i++) {
                        ClueRecord* rec = recs[i];
                        rec->printme(stdout,!nocolor);
                        delete rec;
                    }
                    free(recs);
                    recs = NULL;
                    puts("");
                } 
            }
        }
    }
    poptFreeContext(optCon);
    for (i = 0; i < filect; i++) {
        free((void*) filenames[i]);
    }
    free(filenames);
    return 0;
}
