static HashEntry* record_backtrace(intptr_t* backtrace, size_t numEntries, size_t size)
{
    size_t hash = get_hash(backtrace, numEntries);
    size_t slot = hash % HASHTABLE_SIZE;

    if (size & SIZE_FLAG_MASK) {
        debug_log("malloc_debug: allocation %zx exceeds bit width\n", size);
        abort();
    }

    if (gMallocLeakZygoteChild)
        size |= SIZE_FLAG_ZYGOTE_CHILD;

    HashEntry* entry = find_entry(&gHashTable, slot, backtrace, numEntries, size);

    if (entry != NULL) {
        entry->allocations++;
    } else {
        // create a new entry
        entry = (HashEntry*)dlmalloc(sizeof(HashEntry) + numEntries*sizeof(intptr_t));
        if (!entry)
            return NULL;
        entry->allocations = 1;
        entry->slot = slot;
        entry->prev = NULL;
        entry->next = gHashTable.slots[slot];
        entry->numEntries = numEntries;
        entry->size = size;

        memcpy(entry->backtrace, backtrace, numEntries * sizeof(intptr_t));

        gHashTable.slots[slot] = entry;

        if (entry->next != NULL) {
            entry->next->prev = entry;
        }

        // we just added an entry, increase the size of the hashtable
        gHashTable.count++;
    }

    return entry;
}