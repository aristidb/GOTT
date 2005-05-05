cxx = ARGUMENTS.get('cxx', Environment()['CXX'])
cflags = ARGUMENTS.get('cflags', '-Wall -W')
release = int(ARGUMENTS.get('release', 0))
if release:
	debug = 0
	opt = '-O3'
else:
	debug = 1
	opt = '-O1'
debug = int(ARGUMENTS.get('debug', debug))
opt = ARGUMENTS.get('opt', opt)
cflags += ' ' + opt
if debug:
	cflags += ' -g -ggdb'
	cppdef = ['DEBUG']
else:
	cppdef = ['NDEBUG']
caux = ARGUMENTS.get('aux', '')
cflags += ' ' + caux
cflagsoverride = ARGUMENTS.get('cfoverride', 0)
if cflagsoverride:
	cflags = cflagsoverride

env = Environment(CXX = cxx, CCFLAGS = cflags, CPPPATH='#/..', CPPDEFINES=cppdef)

Export('env')
conf = SConscript('configure/SConscript')
env = conf.Finish()

SConscript(dirs = ['util'])
