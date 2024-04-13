static void insert_case4(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* g = grandparent(n);
    if (n == n->parent->right && n->parent == g->left) {
        rotate_left(n->parent);
        n = n->left;
    } else if (n == n->parent->left && n->parent == g->right) {
        rotate_right(n->parent);
        n = n->right;
    }
    insert_case5(n);
}