static uint32_t get_hash(intptr_t* backtrace, size_t numEntries)
{
    if (backtrace == NULL) return 0;

    int hash = 0;
    size_t i;
    for (i = 0 ; i < numEntries ; i++) {
        hash = (hash * 33) + (backtrace[i] >> 2);
    }

    return hash;
}