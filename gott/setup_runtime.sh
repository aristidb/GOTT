# source me!
export LD_LIBRARY_PATH=$(find $PWD -type d|grep -v \\.svn|xargs -i echo -n {}:)
echo $LD_LIBRARY_PATH
