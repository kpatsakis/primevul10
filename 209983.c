static Jsi_TreeEntry* uncle(Jsi_TreeEntry* n) {
    Assert (n != NULL);
    Assert (n->parent != NULL);
    Assert (n->parent->parent != NULL);
    return sibling(n->parent);
}