static void insert_case5(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* g = grandparent(n);
    set_color(n->parent, _JSI_TREE_BLACK);
    set_color(g, _JSI_TREE_RED);
    if (n == n->parent->left) {
        rotate_right(g);
    } else {
        Assert (n == n->parent->right);
        rotate_left(g);
    }
}