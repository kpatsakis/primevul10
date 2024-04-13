static int node_color(Jsi_TreeEntry* n) {
    return n == NULL ? _JSI_TREE_BLACK : n->f.bits.color;
}