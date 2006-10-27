import glob
files = glob.glob("src/*.cpp")

flags = [ "-Wall", "-W", "-O3" ]
#flags += [ "-g" ]

env = Environment(CPPPATH = [ "#include" ])
env.Program("example",
      files, 
      CXXFLAGS = flags + [ "-Werror" ])

env.Program("example2",
      files,
      CXXFLAGS = flags + [ "-fno-exceptions" ],
      OBJSUFFIX=".ox")
