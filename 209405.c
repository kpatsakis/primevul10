Jsi_MapEntry* Jsi_MapEntryNew (Jsi_Map *mapPtr, const void *key, bool *isNew){
    SIGASSERT(mapPtr, MAP);
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: return (Jsi_MapEntry*)Jsi_HashEntryNew(mapPtr->v.hash, key, isNew);
        case JSI_MAP_TREE: return (Jsi_MapEntry*)Jsi_TreeEntryNew(mapPtr->v.tree, key, isNew);
        case JSI_MAP_LIST: {
            Jsi_ListEntry *lptr = Jsi_ListEntryNew(mapPtr->v.list, NULL, (key?mapPtr->v.list->head:NULL));
            if (isNew) *isNew = 1;
            return (Jsi_MapEntry*)lptr;
        }
        break;
        case JSI_MAP_NONE: break;
    }
    return NULL;
}