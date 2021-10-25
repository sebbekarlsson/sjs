mkdir -p build && \
    cd build && \
    cmake .. && \
    make && \
    cd tests && \
    make && \
    ./test
