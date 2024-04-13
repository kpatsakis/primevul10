void Jsi_ListClear(Jsi_List *list) {
    Jsi_ListEntry *l;
    while (list->head) {
        l = list->head;
        list->head = list->head->next;
        l->next = l->prev = NULL;
        if (list->opts.freeListProc && l->value)
            (list->opts.freeListProc)(list->opts.interp, l, l->value);
        Jsi_ListEntryDelete(l);
    }
    list->numEntries = 0;
}