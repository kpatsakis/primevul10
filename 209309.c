static void insert_case2(Jsi_TreeEntry* n) {
    if (node_color(n->parent) == _JSI_TREE_BLACK)
        return;
    insert_case3(n);
}