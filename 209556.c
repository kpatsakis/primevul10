uint Jsi_MapSize(Jsi_Map *mapPtr) {
    SIGASSERT(mapPtr, MAP);
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: return Jsi_HashSize(mapPtr->v.hash);
        case JSI_MAP_TREE: return Jsi_TreeSize(mapPtr->v.tree);
        case JSI_MAP_LIST: return Jsi_ListSize(mapPtr->v.list);
        case JSI_MAP_NONE: break;
    }
    return -1;
}