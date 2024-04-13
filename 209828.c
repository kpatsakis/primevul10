static void insert_case1(Jsi_TreeEntry* n) {
    if (n->parent == NULL)
        set_color(n, _JSI_TREE_BLACK);
    else
        insert_case2(n);
}