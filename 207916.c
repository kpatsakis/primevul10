void leak_free(void* mem)
{
    if (mem != NULL) {
        pthread_mutex_lock(&gAllocationsMutex);

        // check the guard to make sure it is valid
        AllocationEntry* header = (AllocationEntry*)mem - 1;

        if (header->guard != GUARD) {
            // could be a memaligned block
            if (((void**)mem)[-1] == MEMALIGN_GUARD) {
                mem = ((void**)mem)[-2];
                header = (AllocationEntry*)mem - 1;
            }
        }

        if (header->guard == GUARD || is_valid_entry(header->entry)) {
            // decrement the allocations
            HashEntry* entry = header->entry;
            entry->allocations--;
            if (entry->allocations <= 0) {
                remove_entry(entry);
                dlfree(entry);
            }

            // now free the memory!
            dlfree(header);
        } else {
            debug_log("WARNING bad header guard: '0x%x'! and invalid entry: %p\n",
                    header->guard, header->entry);
        }

        pthread_mutex_unlock(&gAllocationsMutex);
    }
}