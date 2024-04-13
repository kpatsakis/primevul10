void Jsi_ListDelete(Jsi_List *list) {
    Jsi_ListClear(list);
    free(list);
}