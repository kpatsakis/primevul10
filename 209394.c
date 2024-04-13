Jsi_MapEntry* Jsi_MapSearchFirst (Jsi_Map *mapPtr, Jsi_MapSearch *searchPtr, int flags){
    SIGASSERT(mapPtr, MAP);
    searchPtr->typ = mapPtr->typ;
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: return (Jsi_MapEntry*)Jsi_HashSearchFirst(mapPtr->v.hash, &searchPtr->v.hash);
        case JSI_MAP_TREE: return (Jsi_MapEntry*)Jsi_TreeSearchFirst(mapPtr->v.tree, &searchPtr->v.tree, flags, NULL);
        case JSI_MAP_LIST: return (Jsi_MapEntry*)Jsi_ListSearchFirst(mapPtr->v.list, &searchPtr->v.list, flags);
        case JSI_MAP_NONE: break;
    }
    return NULL;
}