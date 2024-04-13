Jsi_Value *jsi_ScopeChainObjLookupUni(jsi_ScopeChain *sc, char *key)
{
    int i;
    Jsi_Value *ret;
    for (i = sc->chains_cnt - 1; i >= 0; --i) {
        if ((ret = Jsi_ValueObjLookup(sc->interp, sc->chains[i], key, 0))) {
            return ret;
        }
    }
    return NULL;
}