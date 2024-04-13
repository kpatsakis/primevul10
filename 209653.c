static void delete_one_child(Jsi_TreeEntry*n)
{
    Jsi_TreeEntry *child;
    Assert(n->left == NULL || n->right == NULL);
    child = n->right == NULL ? n->left  : n->right;
#if 1
    if (node_color(n) == _JSI_TREE_BLACK) {
        set_color(n, node_color(child));
        delete_case1(n);
    }
    replace_node(n, child);
    if (n->parent == NULL && child != NULL)
        set_color(child, _JSI_TREE_BLACK);
    
#else
    replace_node(n, child);
    if (node_color(n) == _JSI_TREE_BLACK) {
        if (node_color(child) == _JSI_TREE_RED)
            child->f.bits.color = _JSI_TREE_BLACK;
        else
            delete_case1(n);
    }
#endif
}