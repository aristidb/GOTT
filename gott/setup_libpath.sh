export LD_LIBRARY_PATH=$(for i in $(cat libdirs.txt); do echo -n $PWD/$i:; done; echo -n .)
