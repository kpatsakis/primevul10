static void rotate_left(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* r;
    Assert(n);
    r = n->right;
    replace_node(n, r);
    n->right = r->left;
    if (r->left != NULL) {
        r->left->parent = n;
    }
    r->left = n;
    n->parent = r;
}