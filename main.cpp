#include <cstdlib>
#include <iostream>

#include <wasm-io.h>

// Tree Visitor
struct CountingVisitor : public wasm::OverriddenVisitor<CountingVisitor, int> {
    int visitBlock(wasm::Block *curr) {
        int count = 0;

        for (const auto &expr : curr->list) {
            count += visit(expr);
        }

        return count;
    }
    int visitIf(wasm::If *curr) {
        int count = 0;

        count += visit(curr->condition);
        count += visit(curr->ifTrue);

        if (curr->ifFalse)
            count += visit(curr->ifFalse);

        return count;
    }
    int visitLoop(wasm::Loop *curr) {
        return visit(curr->body);
    }
    int visitBreak(wasm::Break *curr) {
        int count = 0;

        if (curr->condition)
            count += visit(curr->condition);

        if (curr->value)
            count += visit(curr->value);

        return count;
    }
    int visitSwitch(wasm::Switch *curr) {
        int count = 0;

        count += visit(curr->condition);

        if (curr->value)
            count += visit(curr->value);

        return count;
    }
    int visitCall(wasm::Call *curr) {
        int count = 0;

        for (const auto &expr : curr->operands) {
            count += visit(expr);
        }

        return count;
    }
    int visitCallIndirect(wasm::CallIndirect *curr) {
        int count = 0;

        count += visit(curr->target);

        for (const auto &expr : curr->operands) {
            count += visit(expr);
        }

        return count;
    }
    int visitGetLocal([[maybe_unused]] wasm::GetLocal *curr) {
        return 0;
    }
    int visitSetLocal(wasm::SetLocal *curr) {
        return visit(curr->value);
    }
    int visitGetGlobal([[maybe_unused]] wasm::GetGlobal *curr) {
        return 0;
    }
    int visitSetGlobal(wasm::SetGlobal *curr) {
        return visit(curr->value);
    }
    int visitLoad(wasm::Load *curr) {
        return visit(curr->ptr);
    }
    int visitStore(wasm::Store *curr) {
        return visit(curr->ptr) + visit(curr->value);
    }
    int visitAtomicRMW(wasm::AtomicRMW *curr) {
        return visit(curr->ptr) + visit(curr->value);
    }
    int visitAtomicCmpxchg(wasm::AtomicCmpxchg *curr) {
        return visit(curr->ptr) + visit(curr->expected) + visit(curr->replacement);
    }
    int visitAtomicWait(wasm::AtomicWait *curr) {
        return visit(curr->ptr) + visit(curr->expected) + visit(curr->timeout);
    }
    int visitAtomicNotify(wasm::AtomicNotify *curr) {
        return visit(curr->ptr) + visit(curr->notifyCount);
    }
    int visitSIMDExtract(wasm::SIMDExtract *curr) {
        return visit(curr->vec);
    }
    int visitSIMDReplace(wasm::SIMDReplace *curr) {
        return visit(curr->vec) + visit(curr->value);
    }
    int visitSIMDShuffle(wasm::SIMDShuffle *curr) {
        return visit(curr->left) + visit(curr->right);
    }
    int visitSIMDBitselect(wasm::SIMDBitselect *curr) {
        return visit(curr->cond) + visit(curr->left) + visit(curr->right);
    }
    int visitSIMDShift(wasm::SIMDShift *curr) {
        return visit(curr->vec) + visit(curr->shift);
    }
    int visitMemoryInit(wasm::MemoryInit *curr) {
        return visit(curr->dest) + visit(curr->offset) + visit(curr->size);
    }
    int visitDataDrop([[maybe_unused]] wasm::DataDrop *curr) {
        return 0;
    }
    int visitMemoryCopy(wasm::MemoryCopy *curr) {
        return visit(curr->dest) + visit(curr->source) + visit(curr->size);
    }
    int visitMemoryFill(wasm::MemoryFill *curr) {
        return visit(curr->dest) + visit(curr->value) + visit(curr->size);
    }
    int visitConst([[maybe_unused]] wasm::Const *curr) {
        return 1;
    }
    int visitUnary(wasm::Unary *curr) {
        return visit(curr->value);
    }
    int visitBinary(wasm::Binary *curr) {
        return visit(curr->left) + visit(curr->right);
    }
    int visitSelect(wasm::Select *curr) {
        return visit(curr->condition) + visit(curr->ifTrue) + visit(curr->ifFalse);
    }
    int visitDrop(wasm::Drop *curr) {
        return visit(curr->value);
    }
    int visitReturn(wasm::Return *curr) {
        if (curr->value)
            return visit(curr->value);

        return 0;
    }
    int visitHost(wasm::Host *curr) {
        int count = 0;

        for (const auto &expr : curr->operands) {
            count += visit(expr);
        }

        return count;
    }
    int visitNop([[maybe_unused]] wasm::Nop *curr) {
        return 0;
    }
    int visitUnreachable([[maybe_unused]] wasm::Unreachable *curr) {
        WASM_UNREACHABLE();
    }

    int visitFunctionType([[maybe_unused]] wasm::FunctionType *curr) {
        WASM_UNREACHABLE();
    }
    int visitExport([[maybe_unused]] wasm::Export *curr) {
        WASM_UNREACHABLE();
    }
    int visitGlobal([[maybe_unused]] wasm::Global *curr) {
        WASM_UNREACHABLE();
    }
    int visitFunction([[maybe_unused]] wasm::Function *curr) {
        WASM_UNREACHABLE();
    }
    int visitTable([[maybe_unused]] wasm::Table *curr) {
        WASM_UNREACHABLE();
    }
    int visitMemory([[maybe_unused]] wasm::Memory *curr) {
        WASM_UNREACHABLE();
    }
    int visitModule([[maybe_unused]] wasm::Module *curr) {
        WASM_UNREACHABLE();
    }
};

// Counting Pass Class
class CountingPass : public wasm::Pass {
public:
    // On module object callback
    void run([[maybe_unused]] wasm::PassRunner *pass_runner, wasm::Module *module) override {
        // Create the visitor object
        CountingVisitor visitor;

        // Iterate functions in the module
        int count = 0;

        for (const auto &function : module->functions) {
            count += visitor.visit(function->body);
        }

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
