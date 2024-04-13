static void delete_case1(Jsi_TreeEntry* n) {
    if (n->parent != NULL)
        delete_case2(n);
}