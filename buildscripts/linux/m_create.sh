PROJECT=$1

rm -rf project
mkdir project
cd project

if [ "${PROJECT}" == "r" ]; then
    echo "************ Creating Release Profile ************"
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../src
else
    echo "************ Creating Debug Profile ************"
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../src
fi

# cmake ../src
# -G Xcode -H. -B_build