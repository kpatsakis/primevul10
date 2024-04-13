static void set_color(Jsi_TreeEntry* n, int color) {
    if (color == _JSI_TREE_BLACK && n == NULL) return;
    n->f.bits.color = color;
}