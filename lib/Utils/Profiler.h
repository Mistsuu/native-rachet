#include <chrono>
#define measureTime( tstFunc ) do {                                                  \
    auto start = chrono::high_resolution_clock::now();                               \
    tstFunc                                                                          \
    auto stop  = chrono::high_resolution_clock::now();                               \
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);       \
    std::cerr<< "[i] Time taken to run: " << duration.count() << "us" << std::endl;  \
} while(0)

