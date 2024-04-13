static int tree_inorder(Jsi_Tree *treePtr, Jsi_TreeEntry *hPtr, Jsi_TreeWalkProc *callback, void *data) {
    uint epoch = treePtr->epoch;
    if (hPtr == NULL) return JSI_OK;
    if (hPtr->right != NULL) {
        if (tree_inorder(treePtr, hPtr->right, callback, data) != JSI_OK || epoch != treePtr->epoch)
            return JSI_ERROR;
    }
    if (callback(treePtr, hPtr, data) != JSI_OK || epoch != treePtr->epoch)
        return JSI_ERROR;
    Assert(hPtr->treePtr);
    if (hPtr->left != NULL) {
        if (tree_inorder(treePtr, hPtr->left, callback, data) != JSI_OK || epoch != treePtr->epoch)
            return JSI_ERROR;
    }
    return JSI_OK;
}