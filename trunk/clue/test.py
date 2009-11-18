#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import locale

import os, re, sys, string
from sys import exit
from string import lower

from clue import *

locale.setlocale(locale.LC_ALL, "")

if len(sys.argv) < 1:
    print "usage: bla"
    exit(1);

#filename = sys.argv[1]
filename = "/usr/share/clue/cluknomx.DAT"
cf = ClueFile(filename)

def sorttest():
    tc = 0
    bads = 0
    oldword = ""
    while True:
        rec = cf.getNextRecord()
        if not rec: break
        word = rec.getWord()
        word = strfromlocal(word)
        tc += 1
        if cluecmp(oldword, word) > 0:
            bads += 1
            print strtolocal(oldword), "is greater than", strtolocal(word)
            print strtolocal(mksortcompat(oldword)), "is greater than", strtolocal(mksortcompat(word))
        oldword = word

    print "sortedness: %f" % (1.0 - (float(bads) / tc))
    print "tested: %u" % tc

def binsearch(cf, word):
    lower, upper = 0, cf.length()
    matches = []
    
    debug = True
    debug = False
    
    while True:
        guess = lower + ((upper - lower) / 2)

        cf.seek(guess)
        rec = cf.getPrevRecord()

        cmpres = cluecmp(word, rec.getWord())
        
        if cmpres == 0:
            foundpos = cf.tell()
            while cluecmp(word, rec.getWord()) == 0:
                matches.append(rec)
                rec = cf.getPrevRecord()
            cf.seek(foundpos)
            rec = cf.getNextRecord()
            while cluecmp(word, rec.getWord()) == 0:
                matches.append(rec)
                rec = cf.getNextRecord()
            return matches
        elif cmpres < 0:
            upper = guess
        elif cmpres > 0:
            lower = guess
        
        if abs(lower - upper) <= 1:
            return
            
        if debug:
            rec.printme()
            print "[%u,%u]" % (lower, upper)


def soktest():
    tc = 0
    nom = 0
    fd = file("wordlist.txt", "r")
    for word in fd.xreadlines():
        word = word[:-1]
        rec = cf.binSearch(word)
        #rec = binsearch(cf, word)
        tc += 1
        if rec:
            pass
            #rec.printme()
        else:
            print "no match for:", word
            nom += 1
        
    print nom, "out of", tc, "failed"
    print "accuracy: %f" % (1.0 - (float(nom) / tc))

#soktest()

#recs = binsearch(cf, "abode")
#if recs:
#    for rec in recs:
#        rec.printme()
#else:
#    print "no match"

for file in os.listdir("/usr/share/clue"):
    filename = os.path.join("/usr/share/clue", file)
    print "checking: %s" % filename
    cf = ClueFile(filename)
    sorttest()


