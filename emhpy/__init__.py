#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ft=python ts=4 sw=4 sts=4 et fenc=utf-8

'''
module initializer
'''

import sys
from singleton import Singleton

def usage():
    'print usage'
    print 'usage: %s [options]' % sys.argv[0]

def main():
    'entry point'
    if sys.argv < 1:
        usage()
        sys.exit(1)

if __name__ == '__main__':
    main()

