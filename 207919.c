static void remove_entry(HashEntry* entry)
{
    HashEntry* prev = entry->prev;
    HashEntry* next = entry->next;

    if (prev != NULL) entry->prev->next = next;
    if (next != NULL) entry->next->prev = prev;

    if (prev == NULL) {
        // we are the head of the list. set the head to be next
        gHashTable.slots[entry->slot] = entry->next;
    }

    // we just removed and entry, decrease the size of the hashtable
    gHashTable.count--;
}