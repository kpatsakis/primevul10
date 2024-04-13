static HashEntry* find_entry(HashTable* table, int slot,
        intptr_t* backtrace, size_t numEntries, size_t size)
{
    HashEntry* entry = table->slots[slot];
    while (entry != NULL) {
        //debug_log("backtrace: %p, entry: %p entry->backtrace: %p\n",
        //        backtrace, entry, (entry != NULL) ? entry->backtrace : NULL);
        /*
         * See if the entry matches exactly.  We compare the "size" field,
         * including the flag bits.
         */
        if (entry->size == size && entry->numEntries == numEntries &&
                !memcmp(backtrace, entry->backtrace, numEntries * sizeof(intptr_t))) {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}