static void jsi_PushFunc(jsi_Pstate *ps, jsi_OpCode *ip, jsi_ScopeChain *scope, Jsi_Value *currentScope) {
    /* TODO: now that we're caching ps, may need to reference function ps for context_id??? */
    Jsi_Interp *interp = ps->interp;
    Jsi_FuncObj *fo = jsi_FuncObjNew(interp, (Jsi_Func *)ip->data);
    fo->scope = jsi_ScopeChainDupNext(interp, scope, currentScope);
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_FUNCTION);
    obj->d.fobj = fo;
    
    Jsi_Value *v = _jsi_STACKIDX(interp->framePtr->Sp), *fun_prototype = jsi_ObjValueNew(interp);
    fun_prototype->d.obj->__proto__ = interp->Object_prototype;                
    Jsi_ValueMakeObject(interp, &v, obj);
    Jsi_ValueInsert(interp, v, "prototype", fun_prototype, JSI_OM_DONTDEL|JSI_OM_DONTENUM);
    /* TODO: make own prototype and prototype.constructor */
    
    bool isNew;
    Jsi_HashEntry *hPtr;  Jsi_Value *vv;
    if (interp->framePtr->Sp == 1 && (vv=_jsi_STACKIDX(0))->vt == JSI_VT_VARIABLE) {
        const char *varname = NULL;
        vv = vv->d.lval;
        if (vv && vv->f.bits.lookupfailed && vv->d.lookupFail) {
            varname = vv->d.lookupFail;
            vv->f.bits.lookupfailed = 0;
        }
        if (varname) {
            if (!fo->func->name)
                fo->func->name = varname;
            hPtr = Jsi_HashEntryNew(interp->varTbl, varname, &isNew);
            if (hPtr)
                Jsi_HashValueSet(hPtr, obj);
        }
    }
    hPtr = Jsi_HashEntryNew(interp->funcObjTbl, fo, &isNew);
    if (hPtr && isNew) {
        Jsi_ObjIncrRefCount(interp, obj);
        Jsi_HashValueSet(hPtr, obj);
    }
    jsiPush(interp,1);
}