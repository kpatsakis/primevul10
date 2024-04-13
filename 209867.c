static void delete_case2(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* s = sibling(n);
    if (node_color(s) == _JSI_TREE_RED) {
        set_color(n->parent, _JSI_TREE_RED);
        set_color(s, _JSI_TREE_BLACK);
        if (n == n->parent->left)
            rotate_left(n->parent);
        else
            rotate_right(n->parent);
    }
    delete_case3(n);
}