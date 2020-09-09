PROJECT=$1

rm -rf project
mkdir project
cd project

if [ "${PROJECT}" == "r" ]; then
    echo "************ Creating Release Profile ************"
    cmake -DCMAKE_BUILD_TYPE=Release -DNDEBUG=true ../src
else
    echo "************ Creating Debug Profile ************"
    cmake -DCMAKE_BUILD_TYPE=Debug ../src
fi