static void rotate_right(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* l;
    Assert(n);
    l = n->left;
    replace_node(n, l);
    n->left = l->right;
    if (l->right != NULL) {
        l->right->parent = n;
    }
    l->right = n;
    n->parent = l;
}