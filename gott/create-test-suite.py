#!/usr/bin/env python

scons = '../scons/scons.py'
nm = 'nm'
cxxfilt = 'c++filt'

def compile(file):
    os.system(scons + ' ' + file)

def auto_tests_list(file):
    compile(file);
    pipe = os.popen(nm + ' -j ' + file + ' | ' + cxxfilt)
    import re
    regex = re.compile('.*test[a-zA-Z0-9]*()$') #!!!!!!!
    funcs = []
    for func in pipe:
	if regex.match(func):
	    funcs.append(func)
    return funcs

def manual_tests_list(file):
    try:
	return open(file.replace('.cpp', '.tests'), 'r').readlines()
    except IOError:
	print 'WARNING: No manual test file found'
    return []

def create_suite(file, options):
    test_funcs = []
    if options.auto:
	test_funcs.append(auto_tests_list(file))
    if options.manual:
	test_funcs.append(manual_tests_list(file))

    # TODO check if file already exists
    fh = open(file.replace('.cpp', '-suite.cpp'), 'w')
    
#    for func in test_funcs:
	

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
parser.add_option('-t', '--create-test-runner', action='store_true',
		  dest='update', help='Do not call create-test-runner.py',
		  default=False)

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
