void jsi_ScopeChainFree(Jsi_Interp *interp, jsi_ScopeChain *sc)
{
    int i;
    for (i = 0; i < sc->chains_cnt; ++i) {
        Jsi_DecrRefCount(interp, sc->chains[i]);
    }
    Jsi_Free(sc->chains);
    _JSI_MEMCLEAR(sc);
    Jsi_Free(sc);
}