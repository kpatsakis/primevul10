Jsi_MapEntry* Jsi_MapEntryFind (Jsi_Map *mapPtr, const void *key){
    SIGASSERT(mapPtr, MAP);
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: return (Jsi_MapEntry*)Jsi_HashEntryFind(mapPtr->v.hash, key);
        case JSI_MAP_TREE: return (Jsi_MapEntry*)Jsi_TreeEntryFind(mapPtr->v.tree, key);
        case JSI_MAP_LIST:
            return (Jsi_MapEntry*) (key == NULL? Jsi_ListGetFront(mapPtr->v.list) : Jsi_ListGetBack(mapPtr->v.list));
        case JSI_MAP_NONE: break;
    }
    return NULL;
}