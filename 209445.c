Jsi_TreeEntry *Jsi_TreeEntryFind (Jsi_Tree *treePtr, const void *key)
{
    Jsi_TreeEntry* hPtr = treePtr->root;
    int rc;
    if (treePtr->flags.destroyed)
        return NULL;
    if (treePtr->opts.lockTreeProc && (*treePtr->opts.lockTreeProc)(treePtr, 1) != JSI_OK)
        return NULL;
    while (hPtr != NULL) {
        rc = treePtr->opts.compareTreeProc(treePtr, Jsi_TreeKeyGet(hPtr), key);
        if (rc == 0) {
            break;
        }
        hPtr = (rc < 0 ? hPtr->left : hPtr->right);
    }
    if (treePtr->opts.lockTreeProc)
        (*treePtr->opts.lockTreeProc)(treePtr, 0);
    return hPtr;
}