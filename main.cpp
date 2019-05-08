#include <cstdlib>
#include <iostream>

#include <wasm-io.h>

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " [PATH]" << std::endl;
            return EXIT_FAILURE;
        }

        // Read WebAssembly Module
        wasm::Module module;
        wasm::ModuleReader{}.read(argv[1], module);
    } catch (const wasm::ParseException &e) {
        e.dump(std::cerr);

        return EXIT_FAILURE;
    }
}
