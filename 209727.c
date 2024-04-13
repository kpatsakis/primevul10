static void delete_case3(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* s  = sibling(n);
    if (node_color(n->parent) == _JSI_TREE_BLACK &&
        node_color(s) == _JSI_TREE_BLACK &&
        node_color(s->left) == _JSI_TREE_BLACK &&
        node_color(s->right) == _JSI_TREE_BLACK)
    {
        set_color(s, _JSI_TREE_RED);
        delete_case1(n->parent);
    } else
        delete_case4(n);
}