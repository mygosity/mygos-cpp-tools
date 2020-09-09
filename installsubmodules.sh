git pull --recurse-submodules

cd submodules
cd spdlog && mkdir -p build && cd build
cmake .. && make -j

cd ..
cd ..

cd fmt && mkdir -p build && cd build
cmake .. && make -j

cd ..
cd ..

cd rapidjson && mkdir -p build && cd build
cmake .. && make -j

cd ..
cd ..
