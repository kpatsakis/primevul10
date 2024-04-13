void Jsi_MapClear (Jsi_Map *mapPtr) {
    SIGASSERTV(mapPtr, MAP);
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: Jsi_HashClear(mapPtr->v.hash); break;
        case JSI_MAP_TREE: Jsi_TreeClear(mapPtr->v.tree); break;
        case JSI_MAP_LIST: Jsi_ListClear(mapPtr->v.list); break;
        default: return;
    }
}