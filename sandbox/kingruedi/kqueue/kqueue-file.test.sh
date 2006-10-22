#!/bin/sh

touch foo
./kqueue-file &
sleep .5

echo -e "\n# echo bar > foo"
echo bar > foo
sleep .5

echo -e "\n# chmod a+w foo"
chmod a+w foo

echo -e "\n# mv foo bar"
mv foo bar

echo -e "\n# rm bar"
rm bar
