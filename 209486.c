static int tree_preorder(Jsi_Tree *treePtr, Jsi_TreeEntry *hPtr, Jsi_TreeWalkProc *callback, void *data) {
    uint epoch = treePtr->epoch;
    if (hPtr == NULL) return JSI_OK;
    if (callback(treePtr, hPtr, data) != JSI_OK || epoch != treePtr->epoch)
        return JSI_ERROR;
    if (hPtr->right != NULL) {
        if (tree_preorder(treePtr, hPtr->right, callback, data) != JSI_OK || epoch != treePtr->epoch)
            return JSI_ERROR;
    }
    if (hPtr->left != NULL) {
        if (tree_preorder(treePtr, hPtr->left, callback, data) != JSI_OK || epoch != treePtr->epoch)
            return JSI_ERROR;
    }
    return JSI_OK;
}