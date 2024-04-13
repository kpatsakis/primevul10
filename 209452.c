Jsi_RC Jsi_TreeConf(Jsi_Tree *treePtr, Jsi_MapOpts *opts, bool set)
{
    if (set)
        treePtr->opts = *opts;
    else
        *opts = treePtr->opts;
    return JSI_OK;
}