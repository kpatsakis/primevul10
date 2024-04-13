Jsi_MapEntry* Jsi_MapSet(Jsi_Map *mapPtr, const void *key, const void *value){
    SIGASSERT(mapPtr, MAP);
    Jsi_MapEntry* mptr = NULL;
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: mptr = (Jsi_MapEntry*)Jsi_HashSet(mapPtr->v.hash, (void*)key, (void*)value); break;
        case JSI_MAP_TREE: mptr = (Jsi_MapEntry*)Jsi_TreeSet(mapPtr->v.tree, (void*)key, (void*)value); break;
        case JSI_MAP_LIST: {
            mptr = Jsi_MapEntryNew(mapPtr, key, NULL);
            Jsi_MapValueSet(mptr, (void*)value);
            break;
        }
        case JSI_MAP_NONE: break;
    }
    return mptr;
}