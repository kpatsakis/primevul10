static void delete_case6(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* s = sibling(n);
    set_color(s, node_color(n->parent));
    set_color(n->parent, _JSI_TREE_BLACK);
    if (n == n->parent->left) {
        Assert (node_color(s->right) == _JSI_TREE_RED);
        set_color(s->right, _JSI_TREE_BLACK);
        rotate_left(n->parent);
    }
    else
    {
        //Assert (node_color(s->left) == _JSI_TREE_RED);
        set_color(s->left, _JSI_TREE_BLACK);
        rotate_right(n->parent);
    }
}