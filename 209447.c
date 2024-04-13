void Jsi_TreeClear (Jsi_Tree *treePtr)
{
    SIGASSERTV(treePtr, TREE);
    if (treePtr->opts.lockTreeProc && (*treePtr->opts.lockTreeProc)(treePtr, 1) != JSI_OK)
        return;
    destroy_node(treePtr->opts.interp, treePtr->root);
    treePtr->root = NULL;
    if (treePtr->opts.lockTreeProc)
        (*treePtr->opts.lockTreeProc)(treePtr, 0);
}