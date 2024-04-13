jsi_ScopeChain *jsi_ScopeChainDupNext(Jsi_Interp *interp, jsi_ScopeChain *sc, Jsi_Value *next)
{
    if (!sc) {
        jsi_ScopeChain *nr = jsi_ScopeChainNew(interp, 1);
        nr->chains[0] = next;
        Jsi_IncrRefCount(interp, next);
        nr->chains_cnt = 1;
        return nr;
    }
    jsi_ScopeChain *r = jsi_ScopeChainNew(interp, sc->chains_cnt + 1);
    int i;
    for (i = 0; i < sc->chains_cnt; ++i) {
        r->chains[i] = sc->chains[i];
        Jsi_IncrRefCount(interp, sc->chains[i]);
    }
    r->chains[i] =  next;
    Jsi_IncrRefCount(interp, next);
    r->chains_cnt = i + 1;
    return r;
}