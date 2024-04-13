MOBI_RET mobi_load_rec(MOBIData *m, FILE *file) {
    MOBI_RET ret;
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    MOBIPdbRecord *curr = m->rec;
    while (curr != NULL) {
        MOBIPdbRecord *next;
        size_t size;
        if (curr->next != NULL) {
            next = curr->next;
            size = next->offset - curr->offset;
        } else {
            fseek(file, 0, SEEK_END);
            long diff = ftell(file) - curr->offset;
            if (diff <= 0) {
                debug_print("Wrong record size: %li\n", diff);
                return MOBI_DATA_CORRUPT;
            }
            size = (size_t) diff;
            next = NULL;
        }

        curr->size = size;
        ret = mobi_load_recdata(curr, file);
        if (ret  != MOBI_SUCCESS) {
            debug_print("Error loading record uid %i data\n", curr->uid);
            mobi_free_rec(m);
            return ret;
        }
        curr = next;
    }
    return MOBI_SUCCESS;
}