if [ "$1" == "d" ]; then
    ./project/Debug/mygos_cpp_tools
elif [ "$1" == "r" ]; then
    ./project/Release/mygos_cpp_tools
else
    ./project/mygos_cpp_tools
fi