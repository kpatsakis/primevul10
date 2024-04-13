void Jsi_MapValueSet(Jsi_MapEntry *h, const void *value){
    switch (jsi_GetListType(h)) {
        case JSI_MAP_HASH: return Jsi_HashValueSet((Jsi_HashEntry*)h, (void*)value);
        case JSI_MAP_TREE: return Jsi_TreeValueSet((Jsi_TreeEntry*)h, (void*)value);
        case JSI_MAP_LIST: return Jsi_ListValueSet((Jsi_ListEntry*)h, (void*)value);
        case JSI_MAP_NONE: break;
    }
}