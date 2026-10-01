git clone --recurse-submodules https://github.com/Daniel6702/Visual-Computing-Course.git

cd Assignment1/

cmake -S . -B build

cmake --build build -j$(nproc)

build/Assignment1