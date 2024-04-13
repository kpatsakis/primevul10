int Jsi_ListEntryDelete(Jsi_ListEntry *l) {
    SIGASSERT(l, LISTENTRY);
    if (l->next || l->prev)
        Jsi_ListPop(l->list, l);
    Jsi_Free(l);
    return 1;
}