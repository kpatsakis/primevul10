static Jsi_TreeEntry* grandparent(Jsi_TreeEntry* n) {
    Assert (n != NULL);
    Assert (n->parent != NULL);
    Assert (n->parent->parent != NULL);
    return n->parent->parent;
}