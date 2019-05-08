#include <cstdlib>

#include <iostream>

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " [PATH]" << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Hello, world" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}
