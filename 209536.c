void* Jsi_ListValueGet(Jsi_ListEntry *l) {
    SIGASSERT(l, LISTENTRY);
    return l?l->value:NULL;
}