#include <iostream>

#ifdef _DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif


int main(int argc, char *argv[]) {
    std::cout << "Run the python file `day25.py` because I had no luck with boost! Command to run : `python3 src/day25.py";
    #ifdef _DEBUG
    std::cout << " DEBUG";
    #endif
    std::cout << "`" << std::endl;

    return 0;
}