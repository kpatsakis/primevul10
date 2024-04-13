static int is_valid_entry(HashEntry* entry)
{
    if (entry != NULL) {
        int i;
        for (i = 0 ; i < HASHTABLE_SIZE ; i++) {
            HashEntry* e1 = gHashTable.slots[i];

            while (e1 != NULL) {
                if (e1 == entry) {
                    return 1;
                }

                e1 = e1->next;
            }
        }
    }

    return 0;
}