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

int main() {
    FlushDenormalsToZero();
    
    KGameInstance &instance = KGameInstance::Instance();

    while (true) {
        instance.calcInput(true, false, false, false, 0, 0, 0);
        instance.calc();
    }

    return 0;
}
