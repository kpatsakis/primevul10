static Jsi_TreeEntry* sibling(Jsi_TreeEntry* n) {
    Assert (n != NULL);
    Assert (n->parent != NULL);
    return (n == n->parent->left ? n->parent->right : n->parent->left);
}