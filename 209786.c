Jsi_TreeEntry *Jsi_TreeEntryNew(Jsi_Tree *treePtr, const void *key, bool *isNew)
{
    Jsi_TreeEntry* hPtr;
    bool isn;
    if (treePtr->flags.destroyed)
        return NULL;
    if (treePtr->opts.lockTreeProc && (*treePtr->opts.lockTreeProc)(treePtr, 1) != JSI_OK)
        return NULL;
    treePtr->flags.inserting=1;
    if (treePtr->flags.internstr) {
        Assert(treePtr->keyType == JSI_KEYS_STRINGKEY);
        if (!treePtr->strHash)
            treePtr->strHash = Jsi_HashNew(treePtr->opts.interp, JSI_KEYS_STRING, NULL);
        key = Jsi_HashEntryNew(treePtr->strHash, key, NULL);
    }
    hPtr = treePtr->createProc(treePtr, key, &isn);
    if (isNew)
        *isNew = isn;
    if (isn == 0 || treePtr->flags.nonredblack == 1 || !hPtr) {
        treePtr->flags.inserting=0;
        goto done;
    }
    treePtr->epoch++;
    hPtr->f.bits.color = _JSI_TREE_RED;
    if (treePtr->root == NULL) {
        treePtr->root = hPtr;
    } else {
        Jsi_TreeEntry* n = treePtr->root;
        while (1) {
            int rc = treePtr->opts.compareTreeProc(treePtr, Jsi_TreeKeyGet(n) , key);
            if (rc == 0) {
                Assert(0);
            } else if (rc < 0) {
                if (n->left == NULL) {
                    n->left = hPtr;
                    break;
                } else {
                    n = n->left;
                }
            } else {
                if (n->right == NULL) {
                    n->right = hPtr;
                    break;
                } else {
                    n = n->right;
                }
            }
        }
        hPtr->parent = n;
    }
    insert_case1(hPtr);
    treePtr->flags.inserting = 0;
done:
    if (treePtr->opts.lockTreeProc)
        (*treePtr->opts.lockTreeProc)(treePtr, 0);
    return hPtr;
}