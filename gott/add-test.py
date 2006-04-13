#!/usr/bin/env python

scons = '../contrib/scons/scons.py'
nm = 'nm'
cxxfilt = 'c++filt'

def compile_tests(file):
    os.system(scons + ' ' + file)

def find_tests(file):
    os.system(nm + ' ' + file)

def read_manual_test_list(file):

def create_suite(file, options):
    test_funcs = []
    if options.auto:
	compile_tests(file)
	test_funcs.append(find_tests(file))
    if options.manual:
	test_funcs.append(read_manual_test_list(file))

import optparse

parser = optparse.OptionParser(usage="usage: %prog [options] files ...\n\nSee http://evilhackerdu.de:2500/gott/show/UnitTests",
			       version="%prog version: 0.01");
parser.add_option('--no-c++filt', action='store_false', dest='auto',
		  default=True,
	          help='`c++filt` is not available - no automatic test-suite creation (see Binutils)')
parser.add_option('--no-nm', action='store_false', dest='auto',
		  help='`nm` is not available - no automatic test-suite creation (see Binutils)')
parser.add_option('--no-auto', action='store_false', dest='auto',
                  help='No automatic test-suite creation')
parser.add_option('--no-manual', action='store_false', dest='manual',
		  help='No manual test-suite creation', default=True)
parser.add_option('--no-update', action='store_false', dest='update',
		  help='Do not call update-tests.py', default=True)

(options, args) = parser.parse_args()

if not options.auto and not options.manual:
    print "ERROR: Nothing to do. Neither automatic nor manual test-suite creation was activated\nQuitting ..."
    import sys
    sys.exit(1)

import os

for file in args:
    create_suite(file, options)

#if options.update:
#    call_update_tests()
