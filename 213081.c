static int getreloc_tree(const void *user, const RBNode *n, void *user2) {
        struct getreloc_t *gr = (struct getreloc_t *)user;
        const RBinReloc *r = container_of (n, const RBinReloc, vrb);
        if ((r->vaddr >= gr->vaddr) && (r->vaddr < (gr->vaddr + gr->size))) {
                return 0;
        }

        if (gr->vaddr > r->vaddr) {
                return 1;
        }
        if (gr->vaddr < r->vaddr) {
                return -1;
        }
        return 0;
}