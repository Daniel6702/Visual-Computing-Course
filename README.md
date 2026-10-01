OpenCV documentation: https://docs.opencv.org/4.5.4/d1/dfb/intro.html

OpenGL: https://learnopengl.com/book/book_pdf.pdf



git clone --recurse-submodules https://github.com/Daniel6702/Visual-Computing-Course.git

cd Assignment1/

cmake -S . -B build
cmake --build build -j$(nproc)
build/Assignment1


