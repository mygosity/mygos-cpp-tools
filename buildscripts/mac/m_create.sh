PROJECT=$1

rm -rf project
mkdir project
cd project

if [ "${PROJECT}" == "x" ]; then
    cp -r ../env/ ./env
    cmake -G "Xcode" -H. -DCMAKE_BUILD_TYPE=Debug ../src
elif [ "${PROJECT}" == "r" ]; then
    echo "************ Creating Release Profile ************"
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DNDEBUG=true ../src
else
    echo "************ Creating Debug Profile ************"
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../src
fi

# cmake ../src
# -G Xcode -H. -B_build