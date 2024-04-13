static int tree_levelorder(Jsi_Tree *treePtr, Jsi_TreeEntry *hPtr, Jsi_TreeWalkProc *callback,
    void *data, int curlev, int level, int *cnt) {
    uint epoch = treePtr->epoch;
    if (hPtr == NULL) return JSI_OK;
    if (curlev > level) return JSI_OK;
    if (curlev == level) {
        if (callback(treePtr, hPtr, data) != JSI_OK || epoch != treePtr->epoch)
            return JSI_ERROR;
        (*cnt)++;
    }
    if (hPtr->right != NULL) {
        if (tree_levelorder(treePtr, hPtr->right, callback, data, curlev+1, level, cnt) != JSI_OK || epoch != treePtr->epoch)
            return JSI_ERROR;
    }
    if (hPtr->left != NULL) {
        if (tree_levelorder(treePtr, hPtr->left, callback, data, curlev+1, level, cnt) != JSI_OK || epoch != treePtr->epoch)
            return JSI_ERROR;
    }
    return JSI_OK;
}