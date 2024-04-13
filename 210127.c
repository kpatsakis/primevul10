Jsi_MapEntry* Jsi_MapSearchNext (Jsi_MapSearch *searchPtr){
    switch (searchPtr->typ) {
        case JSI_MAP_HASH: return (Jsi_MapEntry*)Jsi_HashSearchNext(&searchPtr->v.hash);
        case JSI_MAP_TREE: return (Jsi_MapEntry*)Jsi_TreeSearchNext(&searchPtr->v.tree);
        case JSI_MAP_LIST: return (Jsi_MapEntry*)Jsi_ListSearchNext(&searchPtr->v.list);
        case JSI_MAP_NONE: break;
    }
    return NULL;
}