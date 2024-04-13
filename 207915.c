void* leak_realloc(void* oldMem, size_t bytes)
{
    if (oldMem == NULL) {
        return leak_malloc(bytes);
    }
    void* newMem = NULL;
    AllocationEntry* header = (AllocationEntry*)oldMem - 1;
    if (header && header->guard == GUARD) {
        size_t oldSize = header->entry->size & ~SIZE_FLAG_MASK;
        newMem = leak_malloc(bytes);
        if (newMem != NULL) {
            size_t copySize = (oldSize <= bytes) ? oldSize : bytes;
            memcpy(newMem, oldMem, copySize);
            leak_free(oldMem);
        }
    } else {
        newMem = dlrealloc(oldMem, bytes);
    }
    return newMem;
}