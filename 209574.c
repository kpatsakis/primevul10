void Jsi_MapDelete (Jsi_Map *mapPtr) {
    SIGASSERTV(mapPtr, MAP);
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: Jsi_HashDelete(mapPtr->v.hash); break;
        case JSI_MAP_TREE: Jsi_TreeDelete(mapPtr->v.tree); break;
        case JSI_MAP_LIST: Jsi_ListDelete(mapPtr->v.list); break;
        default: return;
    }
    Jsi_Free(mapPtr);
}