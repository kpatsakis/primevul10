static void SwapNodes(Jsi_TreeEntry* n, Jsi_TreeEntry* m)
{
    Jsi_Tree* t = n->treePtr;
    Jsi_TreeEntry *np, *nl, *nr, *mp, *ml, *mr;
    int mpc = 0, npc = 0, col = n->f.bits.color;
    n->f.bits.color = m->f.bits.color;  m->f.bits.color = col;
    np = n->parent; nl = n->left; nr = n->right;
    mp = m->parent; ml = m->left; mr = m->right;
    if (mp) mpc = (mp->left == m ?1 : 2);
    if (np) npc = (np->left == n ?1 : 2);

    n->parent = mp; n->left = ml; n->right = mr;
    m->parent = np; m->left = nl; m->right = nr;
    
    if (np == m) {
        m->parent = n;
        if (mr == n) n->right = m; else n->left = m;
    } else if (mp == n) {
        n->parent = m;
        if (nr == m) m->right = n; else m->left = n;
    }
    /* Fixup back pointers. */
    if (m->left)  m->left->parent  = m;
    if (m->right) m->right->parent = m;
    if (n->left)  n->left->parent  = n;
    if (n->right) n->right->parent = n;
    if (mpc) { if (mpc==1) n->parent->left = n; else  n->parent->right = n;}
    if (npc) { if (npc==1) m->parent->left = m; else  m->parent->right = m; }
    if (n->parent == NULL) {
        t->root = n;
    } else if (m->parent == NULL) {
        t->root = m;
    }
}