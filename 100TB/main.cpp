#include <sys/mman.h>
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {

    size_t allocated_memory_size = (unsigned long) 100 << 40;

    void* addr = mmap(NULL, allocated_memory_size, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    getchar();
    if (munmap(addr, allocated_memory_size) == -1) {
        perror("munmap");
        return 1;
    }

    return 0;
}