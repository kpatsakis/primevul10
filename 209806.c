static Jsi_RC jsi_PushVar(jsi_Pstate *ps, jsi_OpCode *ip, jsi_ScopeChain *scope, Jsi_Value *currentScope, int context_id) {
    Jsi_Interp *interp = ps->interp;
    jsi_FastVar *fvar = (typeof(fvar))ip->data;
    SIGASSERT(fvar,FASTVAR);
    Jsi_Value **dvPtr = &_jsi_STACKIDX(interp->framePtr->Sp), *dv = *dvPtr, *v = NULL;
    if (fvar->context_id == context_id && fvar->ps == ps) {
        v = fvar->lval;
    } else {
        char *varname = fvar->varname;
        v = Jsi_ValueObjLookup(interp, currentScope, varname, 1);
        if (v) {
            fvar->local = 1;
            if (v->vt == JSI_VT_UNDEF) {
                v->d.lookupFail = varname;
                v->f.bits.lookupfailed = 1;
            }
        } else {
            v = jsi_ScopeChainObjLookupUni(scope, varname);
            if (v) 
                fvar->local = 1;
            else {
                /* add to global scope.  TODO: do not define if a right_val??? */
                Jsi_Value *global_scope = scope->chains_cnt > 0 ? scope->chains[0]:currentScope;
                Jsi_Value key = VALINIT, *kPtr = &key; // Note: a string key so no reset needed.
                Jsi_ValueMakeStringKey(interp, &kPtr, varname);
                v = jsi_ValueObjKeyAssign(interp, global_scope, &key, NULL, JSI_OM_DONTENUM);
                if (v->vt == JSI_VT_UNDEF) {
                    v->d.lookupFail = varname;
                    v->f.bits.lookupfailed = 1;
                }
                jsi_ValueDebugLabel(v, "var", varname);
                bool isNew;
                Jsi_HashEntry *hPtr = Jsi_HashEntryNew(interp->varTbl, varname, &isNew);
                if (hPtr && isNew)
                    Jsi_HashValueSet(hPtr, 0);
            }
        }
        
        Jsi_IncrRefCount(interp, v);

    }
    if (dv != v && (dv->vt != JSI_VT_VARIABLE || dv->d.lval != v)) {
        Jsi_ValueReset(interp, dvPtr);
        dv->vt = JSI_VT_VARIABLE;
        SIGASSERT(v, VALUE);
        dv->d.lval = v;
        dv->f.bits.local = (fvar->local);
    }
    SIGASSERT(v, VALUE);
    jsiPush(interp,1);
    return JSI_OK;
}