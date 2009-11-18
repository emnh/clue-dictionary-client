#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ft=python ts=4 sw=4 sts=4 et fenc=utf-8

'''
terminal utilities.
'''

import sys
import curses
from curses import COLOR_BLACK, COLOR_BLUE, COLOR_CYAN, COLOR_GREEN, \
                COLOR_MAGENTA, COLOR_RED, COLOR_WHITE, COLOR_YELLOW

class ANSIColor(object):
    'format strings with ANSI color'

    @staticmethod
    def colorize(string, color):
        '''Color string with given ANSI color name/code. Color constants can be
        found in the curses module.
        '''
        if isinstance(color, basestring):
            color = getattr(curses, 'COLOR_%s' % color.upper())
        string = "\033[1;3%dm%s\033[0;39m" % (color, string)
        return string

# XXX: implement
#    @staticmethod
#    def strip(string):
#        'strip colors from a string'
#        return string

# XXX: implement
#    @staticmethod
#    def convert(string, replace):
#        '''Replace each colored substring by return value of replace(substring,
#        color).'''
#        return string

    @staticmethod
    def generateMethods():
        '''generates the color methods seen in this class.
           could do it dynamically, but why? the ANSI colors don't change.'''
        import curses
        for color in dir(curses):
            if not color.startswith('COLOR'):
                continue
            name = color[len('COLOR_'):].lower()
            print '''
    @classmethod
    def %s(cls, string):
        'color string %s'
        return cls.colorize(string, %s)''' % (name, name, color)

    @classmethod
    def black(cls, string):
        'color string black'
        return cls.colorize(string, COLOR_BLACK)

    @classmethod
    def blue(cls, string):
        'color string blue'
        return cls.colorize(string, COLOR_BLUE)

    @classmethod
    def cyan(cls, string):
        'color string cyan'
        return cls.colorize(string, COLOR_CYAN)

    @classmethod
    def green(cls, string):
        'color string green'
        return cls.colorize(string, COLOR_GREEN)

    @classmethod
    def magenta(cls, string):
        'color string magenta'
        return cls.colorize(string, COLOR_MAGENTA)

    @classmethod
    def red(cls, string):
        'color string red'
        return cls.colorize(string, COLOR_RED)

    @classmethod
    def white(cls, string):
        'color string white'
        return cls.colorize(string, COLOR_WHITE)

    @classmethod
    def yellow(cls, string):
        'color string yellow'
        return cls.colorize(string, COLOR_YELLOW)

    @classmethod
    def testColors(cls):
        'print color test'
        # actually this is here so we're using all the color imports. 
        # pylint is very strict..
        message = 'The lazy fox jumped over the brown rabbit.'
        for color in (COLOR_BLACK, COLOR_BLUE, COLOR_CYAN, COLOR_GREEN, \
                COLOR_MAGENTA, COLOR_RED, COLOR_WHITE, COLOR_YELLOW):
            print cls.colorize(message, color)


def usage():
    'print usage'
    print 'usage: %s [options]' % sys.argv[0]

def main():
    'entry point'
    if sys.argv < 1:
        usage()
        sys.exit(1)
    #ANSIColor.generateMethods()
    print ANSIColor.blue('i\'m blue dabedi dabedai')
    ANSIColor.testColors()

if __name__ == '__main__':
    main()

