static void insert_case3(Jsi_TreeEntry* n) {
    Jsi_TreeEntry *g, *u = uncle(n);
    if (node_color(u) == _JSI_TREE_RED) {
        set_color(n->parent, _JSI_TREE_BLACK);
        set_color(u, _JSI_TREE_BLACK);
        g = grandparent(n);
        set_color(g, _JSI_TREE_RED);
        insert_case1(g);
    } else {
        insert_case4(n);
    }
}