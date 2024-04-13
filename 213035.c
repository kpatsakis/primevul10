R_API RBinReloc *r_core_getreloc(RCore *core, ut64 addr, int size) {
        if (size < 1 || addr == UT64_MAX) {
                return NULL;
        }
        RBNode *relocs = r_bin_get_relocs (core->bin);
        if (!relocs) {
                return NULL;
        }
        struct getreloc_t gr = { .vaddr = addr, .size = size };
        RBNode *res = r_rbtree_find (relocs, &gr, getreloc_tree, NULL);
        return res? container_of (res, RBinReloc, vrb): NULL;
}