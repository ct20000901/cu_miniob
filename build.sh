mkdir -p build && cd build && cmake .. -DENABLE_ASAN=ON -DDEBUG=ON -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ && ASAN_OPTIONS=verify_asan_link_order=0 make -j4
