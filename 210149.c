void* Jsi_MapGet(Jsi_Map *mapPtr, const void *key, int flags){
    SIGASSERT(mapPtr, MAP);
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: return Jsi_HashGet(mapPtr->v.hash, (void*)key, flags);
        case JSI_MAP_TREE: return Jsi_TreeGet(mapPtr->v.tree, (void*)key, flags);
        case JSI_MAP_LIST: {
            Jsi_ListEntry* lptr = (key == NULL? Jsi_ListGetFront(mapPtr->v.list) : Jsi_ListGetBack(mapPtr->v.list));
            if (lptr)
                return Jsi_ListValueGet(lptr);
            break;
        }
        case JSI_MAP_NONE: break;
    }
    return NULL;
}