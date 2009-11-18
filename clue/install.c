#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>

#include "clue.h"

/*
   The clue data files are processed like this:
    original clue installer:
        xor decrypts file
        reverses blocks in file
        copies file to installation dir
    original clue program:
        decrypt with character table and some bit fiddling
*/

// malloc with error checking
void* xalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        perror("malloc");
        exit(1);
    }
    return ptr;
}

// Decrypt a block by applying the xorcode and reversing the block.
unsigned char* deXorBlock(const unsigned char* block, uint blocklen, 
                          const unsigned char* xorcode) {
    uint i;
    size_t xorlen = strlen((char*) xorcode);
    uchar* destblock = (uchar*) xalloc(blocklen);
    for (i = 0; i < blocklen; i++) {
        unsigned char chr = block[i];
        //xor decrypt character
        chr = xorcode[i % xorlen] ^ chr;
        //each 64k block is reversed.. stupid obscurity :)
        //write block in reversed order
        destblock[blocklen - 1 - i] = chr;
    }
    return destblock;
}

/* Create the xor code based on filename.
   We DO need the original filename of the clue data file.
   xorcode[i] is (character position + 0x12) xor'ed with character. */
unsigned char* makeXorCode(char* filename) {
    unsigned char* xorcode = NULL;
    uint i, flen;
    flen = strlen(filename);
    xorcode = (uchar*) xalloc(flen + 1);
    for (i = 0; i < strlen(filename); i++) {
        unsigned char chr = (unsigned char) filename[i];
        chr = toupper(chr);
        xorcode[i] = (unsigned char) (i + 0x12) ^ chr;
        if (xorcode[i] == 0) {
            fputs("error: xorcode contains 0 (string terminator)", stderr);
            exit(1);
        }
    }
    xorcode[i] = 0;
    return xorcode;
}

void deCryptFile(const char* filename) {
    unsigned char* buffer;
    unsigned char* destblock;
    unsigned char* xorcode;
    struct stat stat_buf;
    FILE* fd;
    char* bname;
    ssize_t readct;
    uint blockpos, insize, outsize;

    bname = xbasename(filename);
    xorcode = makeXorCode(bname);
    free(bname);
        
    fd = fopen(filename, "r");
    if (fd == NULL) err(1, "%s", filename);
    
    //get optimal read/write chunk sizes
    stat(filename, &stat_buf);
    insize = stat_buf.st_blksize;
    fstat(STDOUT_FILENO, &stat_buf);
    outsize = stat_buf.st_blksize;

    buffer = (uchar*) xalloc(BFS);
    blockpos = 0;
    readct = 1; // > 0
    while (!feof(fd)) {
        uint buffree = BFS - blockpos;
        uint readsize = insize < buffree ? insize : buffree;

        //read until block is full
        readct = fread(&buffer[blockpos], 1, readsize, fd);
        if (ferror(fd)) err(1, "%s", filename);
        blockpos += readct;

        //decrypt and write block if it is full or at end of file
        if (blockpos == BFS || feof(fd)) {
            uint blocksize = blockpos;
            blockpos = 0;
            destblock = deXorBlock(buffer, blocksize, xorcode);
            //write block
            while (blockpos < blocksize) {
                uint bufleft = blocksize - blockpos;
                uint writesize = outsize < bufleft ? outsize : bufleft;
                uint writect;
                writect = fwrite(&destblock[blockpos], 1, writesize, stdout);
                if (ferror(fd)) err(1, "%s", filename);
                blockpos += writect;
            }
            free(destblock);
            if (blockpos != blocksize) {
                fputs("what?????? blockpos != blocksize", stderr);
                exit(1);
            }
            blockpos = 0;
        }
    }
    fclose(fd);
}
