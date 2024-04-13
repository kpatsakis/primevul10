int Jsi_TreeEntryDelete (Jsi_TreeEntry *entryPtr)
{
    int cnt = 0;
    Jsi_TreeEntry* n = entryPtr;
    Jsi_Tree* treePtr = n->treePtr;

    if (treePtr->flags.destroyed  || treePtr->flags.nonredblack == 1 /* || entryPtr->f.bits.deletesub */) {
        goto dodel;
    }
    /*printf("DEL(tree=%p,root=%p): (%p)%s\n", treePtr, treePtr->root, entryPtr,(char*)entryPtr->key.string);*/
    /*dumpTree(treePtr);*/
    if (treePtr->opts.lockTreeProc && (*treePtr->opts.lockTreeProc)(treePtr, 1) != JSI_OK)
        return -1;
    entryPtr->treePtr->epoch++;
    if (n->left != NULL && n->right != NULL) {
        /* swap key/values delete pred instead */
        Jsi_TreeEntry* pred = maximum_node(n->left);
        switch (treePtr->keyType) {
        case JSI_KEYS_STRINGKEY:
        case JSI_KEYS_ONEWORD: {
            void *nv = n->value;
            n->value = pred->value;
            pred->value = nv;
            nv = n->key.oneWordValue;
            n->key.oneWordValue = pred->key.oneWordValue;
            pred->key.oneWordValue = nv;
            n = pred;
            break;
        }
        case JSI_KEYS_STRING:
            SwapNodes(n,pred);
            break;
        default: { // Struct keys have the same length so we swap bytes.
            uint i;
            void *nv = n->value;
            n->value = pred->value;
            pred->value = nv;
            char ct, *cs = (char*)(n->key.string), *cd = (char*)(pred->key.string);
            for (i=0; i<treePtr->keyType; i++, cs++, cd++) {
                ct = *cd;
                *cd = *cs;
                *cs = ct;
            }
        }
                
        }
    }
    delete_one_child(n);
    cnt++;
    /*dumpTree(treePtr);*/
dodel:
    treePtr->numEntries--;
    n->treePtr = NULL;
    if (treePtr->opts.freeTreeProc && n && n->value)
        (treePtr->opts.freeTreeProc)(treePtr->opts.interp, n, n->value);
    Jsi_Free(n);
    if (treePtr->opts.lockTreeProc)
        (*treePtr->opts.lockTreeProc)(treePtr, 0);
    return cnt;
}