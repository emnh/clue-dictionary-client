#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ft=python ts=4 sw=4 sts=4 et fenc=utf-8
# pylint: disable-msg=I0011,W0142

'''
singleton metaclass
'''

import sys

class Singleton(type):
    'Singleton metaclass. Safe for subclassing.'

    def __init__(mcs, name, bases, attrs):
        super(Singleton, mcs).__init__(name, bases, attrs)
        mcs.instance = None

    def __call__(mcs, *args, **kw):
        if mcs.instance is None:
            mcs.instance = super(Singleton, mcs).__call__(*args, **kw)
        return mcs.instance

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

