int jsi_treeHeight(Jsi_TreeEntry* hPtr, int n)
{
    int l = -1, r = -1;
    if (hPtr->right == NULL && hPtr->right == NULL )
        return n;
    if (hPtr->left)
        l = jsi_treeHeight(hPtr->left, n+1);
    if (hPtr->right)
        r = jsi_treeHeight(hPtr->right, n+1);
    return (r > l ? r : l);
}