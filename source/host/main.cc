//#include "host/System.hh"
#include "kgameinstance/KGameInstance.hh"

#if defined(__arm64__) || defined(__aarch64__)
static void FlushDenormalsToZero() {
    uint64_t fpcr;
    asm("mrs %0,   fpcr" : "=r"(fpcr));
    asm("msr fpcr, %0" ::"r"(fpcr | (1 << 24)));
}
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>

static void FlushDenormalsToZero() {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
}
#endif

int main(int argc, char **argv) {
    FlushDenormalsToZero();

    // Avoid unused variable warnings
    (void)argc;
    (void)argv;

    KGameInstance &instance = KGameInstance::Instance();
    instance.initMemory();
    instance.init();

    while (true) {
        // Does nothing for the moment
        instance.calc();
    }

    //return Host::KSystem::Instance().main(argc, argv);
    return 0;
}
