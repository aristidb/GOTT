# source me!
export LD_LIBRARY_PATH=$(find $PWD -name 'lib*.so'|sed -e 's/\/[^/]*$//'|xargs -i echo -n {}:)$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH
