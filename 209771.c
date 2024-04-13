static void delete_case5(Jsi_TreeEntry* n) {
    Jsi_TreeEntry* s = sibling(n);
    if (node_color(s) == _JSI_TREE_BLACK ) {
        if (n == n->parent->left &&
                node_color(s->right) == _JSI_TREE_BLACK &&
                node_color(s->left) == _JSI_TREE_RED)
        {
            set_color(s, _JSI_TREE_RED);
            set_color(s->left, _JSI_TREE_BLACK);
            rotate_right(s);
        }
        else if (n == n->parent->right &&
                 node_color(s->right) == _JSI_TREE_RED &&
                 node_color(s->left) == _JSI_TREE_BLACK)
        {
            set_color(s, _JSI_TREE_RED);
            set_color(s->right, _JSI_TREE_BLACK);
            rotate_left(s);
        }
    }
    delete_case6(n);
}