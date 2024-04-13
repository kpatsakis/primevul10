int Jsi_TreeWalk(Jsi_Tree* treePtr, Jsi_TreeWalkProc* callback, void *data, int flags) {
    Jsi_Interp *interp = treePtr->opts.interp;
    JSI_NOTUSED(interp);
    int n = 0, m = -1, lastm, order;
    order = flags & JSI_TREE_ORDER_MASK;
    switch (order) {
    case JSI_TREE_ORDER_PRE:
        return tree_preorder(treePtr, treePtr->root, callback, data);
    case JSI_TREE_ORDER_POST:
        return tree_postorder(treePtr, treePtr->root, callback, data);
    case JSI_TREE_ORDER_IN:
        return tree_inorder(treePtr, treePtr->root, callback, data);
    case JSI_TREE_ORDER_LEVEL:
        while (1) {
            lastm = m;
            if (tree_levelorder(treePtr, treePtr->root, callback, data, 0, n, &m) != JSI_OK)
                return JSI_ERROR;
            if (lastm == m)
                return JSI_OK;
            n++;
        }
            
    default:
        Jsi_LogError("Invalid order: %d", order);
    }
    return JSI_ERROR;
}