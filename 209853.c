jsi_ScopeChain *jsi_ScopeChainNew(Jsi_Interp *interp, int cnt)
{
    jsi_ScopeChain *r = (jsi_ScopeChain *)Jsi_Calloc(1, sizeof(*r));
    r->interp = interp;
    SIGINIT(r,SCOPE);
    r->chains = (Jsi_Value **)Jsi_Calloc(cnt, sizeof(r->chains[0]));
    r->chains_cnt = cnt;
    return r;
}