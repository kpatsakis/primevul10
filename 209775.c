Jsi_Value *Jsi_VarLookup(Jsi_Interp *interp, const char *varname)
{
    Jsi_Value *v;
    v = Jsi_ValueObjLookup(interp, interp->framePtr->incsc, (char*)varname, 0);
    if (!v)
        v = jsi_ScopeChainObjLookupUni(interp->framePtr->ingsc, (char*)varname);
    return v;
}