Jsi_Tree *Jsi_TreeNew(Jsi_Interp *interp, unsigned int keyType, Jsi_TreeDeleteProc *freeProc)
{
    Jsi_Tree* treePtr = (Jsi_Tree*)Jsi_Calloc(1,sizeof(Jsi_Tree));
    SIGINIT(treePtr,TREE);
    treePtr->opts.mapType = (Jsi_Map_Type)JSI_MAP_TREE;
    treePtr->typ = (Jsi_Map_Type)JSI_MAP_TREE;
    treePtr->root = NULL;
    treePtr->opts.interp = interp;
    treePtr->numEntries = 0;
    treePtr->epoch = 0;
    treePtr->opts.keyType = (Jsi_Key_Type)keyType;
    treePtr->keyType = (Jsi_Key_Type)keyType;
    treePtr->opts.freeTreeProc = freeProc;

    switch (keyType) {
    case JSI_KEYS_STRING:   /* NULL terminated string keys. */
        treePtr->opts.compareTreeProc = StringCompare;
        treePtr->createProc = TreeStringCreate;
        break;

    case JSI_KEYS_STRINGKEY: /*  */
        treePtr->opts.compareTreeProc = StringPtrCompare;
        treePtr->createProc = StringPtrCreate;
        break;
        
    case JSI_KEYS_ONEWORD: /* 32 or 64 bit atomic keys. */
        treePtr->opts.compareTreeProc = OneWordCompare;
        treePtr->createProc = OneWordCreate;
        break;


    default:            /* Struct. */
        if (keyType < JSI_KEYS_STRUCT_MINSIZE) {
            Jsi_LogError("Jsi_TreeNew: Key size can't be %d, must be > %d", keyType, JSI_KEYS_STRUCT_MINSIZE);
            Jsi_Free(treePtr);
            return NULL;
        }
        treePtr->opts.compareTreeProc = TreeArrayCompare;
        treePtr->createProc = TreeArrayCreate;
        break;
    }
    return treePtr;
}