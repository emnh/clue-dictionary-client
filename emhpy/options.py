#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ft=python ts=4 sw=4 sts=4 et fenc=utf-8

'''
options for classes. example:

class Hello(object):
    class Options(OptionsBase):
        person = Option()

    def __init__(self, options):
        assert isinstance(options, self.Options)
        options.validate()
        self.options = options

    def sayHello(self):
        print 'Hello %s!' % person
        
helloOptions = Hello.Options()
helloOptions.person = 'Eivind'
hello = Hello(helloOptions)
hello.sayHello()
'''

import sys

class Option(object):
    'Denotes option for class __init__'
    def __init__(self, required=True, doc=''):
        self.required = required
        self.doc = doc

# XXX: should be abstract class
class OptionsBase(object):
    'Represents __init__ options for another class'

    class OptionsIterator(type):
        'iterate over Option instances in Options class'
        def __iter__(mcs):
            for name in dir(mcs):
                clsvalue = getattr(mcs, name)
                if isinstance(clsvalue, Option):
                    yield clsvalue

    __metaclass__ = OptionsIterator

    def __init__(self):
        'initalize optional options to None'
        for option in type(self):
            if not option.required:
                setattr(self, option.name, None)

    def validate(self):
        'check that all required options are present and initialize optional'
        for option in type(self):
            if not hasattr(self, option.name):
                raise AttributeError('missing option %s for %s' % (
                    option, type(self)))

    def __str__(self):
        ret = ''
        for option in self:
            try:
                value = getattr(self, option) 
                ret += '%s = %s\n' % (option, value)
            except AttributeError:
                ret += '%s missing\n' % (option)
        return ret
         
class OptionsHelper(type):
    '''Generates the docstring for the Options class.
       XXX: Generates a default __init__ method for a class containing Options
    '''
    def __init__(mcs, name, bases, attrs):
        super(OptionsHelper, mcs).__init__(name, bases, attrs)
        mcs.Options.__doc__ += '\n'
        for key in dir(mcs.Options):
            value = getattr(mcs.Options, key)
            if isinstance(value, Option):
                option = value
                option.name = key
                if option.required:
                    mcs.Options.__doc__ += '%s - %s\n' % (
                            option.name, option.doc)
                else:
                    mcs.Options.__doc__ += '[%s] - %s\n' % (
                            option.name, option.doc)

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

