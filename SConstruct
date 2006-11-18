import glob
files = glob.glob("src/*.cpp")

boostpath= "/usr/local/include/boost-1_33_1"

include= boostpath


flags = [ "-Wall", "-W", "-O2" ]

env = Environment(CPPPATH = [ "#include", include])
env.Program("example",
      files, 
      CXXFLAGS = flags + [ "-Werror" ])

env.Program("example2",
      files,
      CXXFLAGS = flags + [ "-fno-exceptions" ],
      OBJSUFFIX=".ox")
