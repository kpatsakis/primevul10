void Jsi_MapSearchDone (Jsi_MapSearch *searchPtr){
    switch (searchPtr->typ) {
        case JSI_MAP_HASH: break;
        case JSI_MAP_TREE: Jsi_TreeSearchDone(&searchPtr->v.tree); break;
        case JSI_MAP_LIST: break;
        case JSI_MAP_NONE: break;
    }
}