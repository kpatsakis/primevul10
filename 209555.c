int jsi_nodeDepth(Jsi_TreeEntry* hPtr) {
    int d = 0;
    while (hPtr->parent != NULL) {
        d++;
        hPtr = hPtr->parent;
    }
    return d;
}