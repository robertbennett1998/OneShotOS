#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define nullptr 0

bool CreateHeap(size_t szHeapSize, uint8_t ubyAlignment);
void* Allocate(size_t szSize);
void Free(void* pPtr);

#endif