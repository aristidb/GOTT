#!/usr/bin/python

import os
import sys
import re
from ftplib import FTP

datakeys= ['workingcopydir',
           'host',
	   'username',
	   'password',
	   'source',
	   'targetdir',
	   'target',
	   'database',
	   'rsstitle',
	   'projectlink',
	   'webmasteremail'
	  ]

def install():
    if os.path.exists(get_my_home()):
       os.system("rm -rf %s" % get_my_home())
    os.mkdir( "%s" % get_my_home() , 0700 )
    os.system("g++ mtlog2rss.cc -o %s/mtlog2rss -lboost_date_time-gcc" % get_my_home())
    f = open('%s/config' % get_my_home(),'w')
    for key in datakeys:
        f.write('%s=\n' % key )
    f.close()
    if os.environ.has_key('EDITOR') and len(os.environ['EDITOR'].strip()) > 0:
       os.system("%s %s/config" % ( os.environ["EDITOR"] , get_my_home() ))
    else :
       print '\n\n\t$EDITOR NOT SET!\n\tPLEASE EDIT %s/config RIGHT NOW!\n\n' % get_my_home()


def get_user_home():
    return os.environ['HOME'];

def get_my_home():
    return "%s/.mtlog2rss" % get_user_home()

def get_data():
    f = open('%s/config' % get_my_home() ,'r')
    data = dict()
    for line in f:
        line = line.replace('\n','')
        line_data = line.split('=')
	data[line_data[0]] = line_data[1]
    f.close()
    return data 
    
def upload(data):
    ftp = FTP(data['host'])
    ftp.login(data['username'],data['password'])
    ftp.cwd(data['targetdir'])
    f = open("%s/changelog.rss" % get_my_home(),'r')
    ftp.storlines('STOR %s' % data['target'],f)
    f.close()
    ftp.quit()


# Reads the log directly from monotone
def read_log( data ):
    os.chdir(data['workingcopydir'])
    if not os.path.exists(data['database']):
       sys.exit("\n\n\n\tExiting with error [ database file not found - config broken ? ]\n\n\n")
    fd = os.popen('monotone --db=%s --quiet log' %data['database'])
    output = fd.read()
    if fd.close():
       sys.exit("\n\n\n\tExiting with error [ monotone read error - database locked? ]\n\n\n")
    return output

def mtlog2rss():
    os.system("%s/mtlog2rss %s/log.txt %s/changelog.rss %s/config" % (get_my_home(),get_my_home(),get_my_home(),get_my_home()))


def emit():
    data = get_data()
    f = open('%s/log.txt' % get_my_home() , 'w')
    f.write(read_log(data))
    f.close()
    mtlog2rss()
    upload(data)


def main():
    if len(sys.argv) == 2:
      if sys.argv[1] == 'install':
         install()
    else:
      emit()

main()

