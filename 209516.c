JSI_RC mycall(Jsi_Tree* treePtr, Jsi_TreeEntry* hPtr, void *data)
{
    printf("CALL: %s(%d) : %d\n", (char*)Jsi_TreeKeyGet(hPtr), jsi_nodeDepth(hPtr), (int)Jsi_TreeValueGet(hPtr));
    return JSI_OK;
}