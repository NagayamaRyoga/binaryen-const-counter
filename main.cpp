#include <cstdlib>
#include <iostream>

#include <wasm-io.h>

// Counting Pass Class
class CountingPass : public wasm::Pass {
public:
    // On module object callback
    void run([[maybe_unused]] wasm::PassRunner *pass_runner, wasm::Module *module) override {
        int count = 0;

        (void)module;

        std::cout << count << " const instruction(s)" << std::endl;
    }

    bool modifiesBinaryenIR() override {
        return false;
    }
};

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " [PATH]" << std::endl;
            return EXIT_FAILURE;
        }

        // Read WebAssembly Module
        wasm::Module module;
        wasm::ModuleReader{}.read(argv[1], module);

        // Register passes to a pass runner
        wasm::PassRunner pass_runner{&module};
        pass_runner.add<CountingPass>();
        pass_runner.run();
    } catch (const wasm::ParseException &e) {
        e.dump(std::cerr);

        return EXIT_FAILURE;
    }
}
