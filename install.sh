PLATFORM="mac"

git submodule update --init --recursive

function installSubmodules() {
    usemake=$1
    echo "*************** installing submodules for mac/linux *************** "
    git pull --recurse-submodules

    cd submodules

    cd spdlog
    mkdir build
    cd build

    if [ "$usemake" == "1" ]; then
        cmake .. && make -j
    else
        cmake ..
    fi

    cd ../../

    cd fmt
    mkdir build
    cd build

    if [ "$usemake" == "1" ]; then
        cmake .. && make -j
    else
        cmake ..
    fi

    cd ../../../
}

case $1 in
    w)  echo "*************** installing windows scripts *************** "
        PLATFORM="windows"
        echo " PLEASE NOTE: You must install spdlog, fmt, rapidjson separately in visual studio"
        installSubmodules
        
        git clone https://github.com/Microsoft/vcpkg.git
        cd vcpkg
        bootstrap-vcpkg.bat
        vcpkg.exe integrate install
        vcpkg.exe install drogon:x64-windows
        cd ..

        ;;
    m)  echo "*************** installing mac scripts *************** "
        PLATFORM="mac"
        installSubmodules 1
        ;;
    l)  echo "*************** installing linux scripts - assuming ubuntu18.04 *************** "
        PLATFORM="linux"
        installSubmodules 1
        # drogon install requirements
        sudo apt install gcc g++ cmake libjsoncpp-dev uuid-dev openssl libssl-dev zlib1g-dev -y
        ;;
    *)
        echo "*************** building default mac scripts *************** "
        installSubmodules 1
        ;;
esac

if [ "$PLATFORM" != "windows" ]; then
    cp ./buildscripts/${PLATFORM}/m_fullbuild.sh ./fullbuild.sh
    cp ./buildscripts/${PLATFORM}/m_make.sh ./make.sh
    cp ./buildscripts/${PLATFORM}/m_mrun.sh ./mrun.sh

    # drogon install requirements https://drogon.docsforge.com/master/installation/
    cd ..
    git clone https://github.com/an-tao/drogon
    cd drogon
    git submodule update --init
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. 
    make && sudo make install
fi

cp ./buildscripts/${PLATFORM}/m_create.sh ./create.sh
cp ./buildscripts/${PLATFORM}/m_run.sh ./run.sh

mkdir _logs
mkdir env
mkdir project
echo "{}" > ./env/env.json
echo "{\"logging\":{\"filewriting\":true}}" > ./env/settings.json
