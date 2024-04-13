static Jsi_TreeEntry* maximum_node(Jsi_TreeEntry* n) {
    Assert (n != NULL);
    while (n->right != NULL) {
        n = n->right;
    }
    return n;
}