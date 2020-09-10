PROJECT=$1
BASEPATH=$PWD

rm -rf project
mkdir project
cd project
if [ "${PROJECT}" == "r" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$BASEPATH/vcpkg/scripts/buildsystems/vcpkg.cmake ../src
else
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$BASEPATH/vcpkg/scripts/buildsystems/vcpkg.cmake ../src
fi
