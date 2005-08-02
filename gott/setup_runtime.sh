# source me!
export LD_LIBRARY_PATH=$(find $PWD -name lib*.so|sed -e 's/\/[^/]*$//')
echo $LD_LIBRARY_PATH
