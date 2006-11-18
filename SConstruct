import glob
files = glob.glob("src/*.cpp")

boost_path= "/usr/local/include/boost-1_33_1"

flags = [ "-Wall", "-W", "-O2" ]

env = Environment(CPPPATH = [ "#include", boost_path])
env.Program("example",
      files, 
      CXXFLAGS = flags + [ "-Werror" ])

env.Program("example2",
      files,
      CXXFLAGS = flags + [ "-fno-exceptions" ],
      OBJSUFFIX=".ox")
