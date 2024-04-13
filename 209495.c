void Jsi_ListValueSet(Jsi_ListEntry *l, const void *value) {
    SIGASSERTV(l, LISTENTRY);
    l->value = (void*)value;
}