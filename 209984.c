Jsi_RC Jsi_MapConf(Jsi_Map *mapPtr, Jsi_MapOpts *opts, bool set)
{
    switch (mapPtr->typ) {
        case JSI_MAP_HASH: return Jsi_HashConf(mapPtr->v.hash, opts, set);
        case JSI_MAP_TREE: return Jsi_TreeConf(mapPtr->v.tree, opts, set);
        case JSI_MAP_LIST: return Jsi_ListConf(mapPtr->v.list, opts, set);
        case JSI_MAP_NONE: break;
    }
    return JSI_ERROR;
}