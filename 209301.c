static void destroy_node(Jsi_Interp *interp, Jsi_TreeEntry* n)
{
    if (n == NULL) return;
    if (n->right != NULL) destroy_node(interp, n->right);
    if (n->left != NULL) destroy_node(interp, n->left);
    n->left = n->right = NULL;
    Jsi_TreeEntryDelete(n);
}