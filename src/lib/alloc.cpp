
#include "lib.h"
#include <atomic>

std::atomic<i64> allocs = 0;

u8* base_alloc(usize sz) {
    u8* ret = (u8*)calloc(sz, 1);
    assert(ret);
    allocs++;
    return ret;
}

void base_free(void* mem) {
    if(mem) {
        free(mem);
        allocs--;
    }
}

void mem_validate() {
    if(allocs != 0) {
        warn("Unbalanced allocations: %", allocs.load());
    } else {
        info("No memory leaked.");
    }
}
