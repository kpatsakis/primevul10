static void replace_node(Jsi_TreeEntry* oldn, Jsi_TreeEntry* newn) {
    Assert(oldn);
    Jsi_Tree* t = oldn->treePtr;
    if (oldn->parent == NULL) {
        t->root = newn;
    } else {
        if (oldn == oldn->parent->left)
            oldn->parent->left = newn;
        else
            oldn->parent->right = newn;
    }
    if (newn != NULL) {
        newn->parent = oldn->parent;
    }
}