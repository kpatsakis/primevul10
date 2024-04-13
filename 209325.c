static Jsi_RC InterpAliasCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    InterpObj *udf = (InterpObj *)Jsi_UserObjGetData(interp, _this, funcPtr);
    Jsi_Interp *sinterp = (udf ? udf->subinterp : interp );
    Jsi_Hash *aliases = sinterp->aliasHash;
    if (!aliases)
        return Jsi_LogError("Sub-interp gone");
    int argc = Jsi_ValueGetLength(interp, args);
    if (argc == 0)
        return Jsi_HashKeysDump(interp, aliases, ret, 0);
    Jsi_HashEntry *hPtr;
    char *key = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    if (!key)
        return Jsi_LogError("expected string");
    AliasCmd* ac;
    if (argc == 1) {
        hPtr = Jsi_HashEntryFind(aliases, (void*)key);
        if (!hPtr)
            return JSI_OK;
        ac = (AliasCmd*)Jsi_HashValueGet(hPtr);
        if (!ac) return JSI_ERROR;
        SIGASSERT(ac,ALIASCMD);
        Jsi_ValueDup2(interp, ret, ac->func);
        return JSI_OK;
    }
    Jsi_Value *afunc = Jsi_ValueArrayIndex(interp, args, 1);
    if (argc == 2) {
        hPtr = Jsi_HashEntryFind(aliases, (void*)key);
        if (!hPtr)
            return JSI_OK;
        ac = (AliasCmd*)Jsi_HashValueGet(hPtr);
        if (!Jsi_ValueIsNull(interp, afunc))
            return Jsi_LogError("arg 2: expected null to query args");
        if (!ac) return JSI_ERROR;
        SIGASSERT(ac,ALIASCMD);
        Jsi_ValueDup2(interp, ret, ac->args); //TODO: JSON??
        return JSI_OK;
    }
    
    if (argc < 3)
        return JSI_ERROR;
    bool isthrd = (interp->threadId != sinterp->threadId);
    //if (isthrd)
        //return Jsi_LogError("alias not supported with threads");
    bool isNew;
    Jsi_Value *aargs = Jsi_ValueArrayIndex(interp, args, 2);
    if (Jsi_ValueIsNull(interp, afunc) && Jsi_ValueIsNull(interp, aargs)) {
        hPtr = Jsi_HashEntryFind(aliases, (void*)key);
        if (hPtr == NULL)
            return JSI_OK;
        ac = (AliasCmd*)Jsi_HashValueGet(hPtr);
        if (!ac) return JSI_ERROR;
        if (0 && ac->cmdVal)
            Jsi_DecrRefCount(interp, ac->cmdVal);
        jsi_AliasFree(interp, NULL, ac);
        Jsi_HashValueSet(hPtr, NULL);
        Jsi_HashEntryDelete(hPtr);
        return JSI_OK;
    }
    hPtr = Jsi_HashEntryNew(aliases, (void*)key, &isNew);
    if (!hPtr)
        return Jsi_LogError("create failed: %s", key);
    if (!Jsi_ValueIsFunction(interp, afunc))
        return Jsi_LogError("arg 2: expected function");
    if (Jsi_ValueIsNull(interp, aargs) == 0 && Jsi_ValueIsArray(interp, aargs) == 0)
        return Jsi_LogError("arg 3: expected array or null");
    if (!isNew) {
        jsi_AliasFree(interp, NULL, Jsi_HashValueGet(hPtr));
    }
    ac = (AliasCmd*)Jsi_Calloc(1, sizeof(AliasCmd));
    SIGINIT(ac, ALIASCMD);
    ac->cmdName = (const char*)Jsi_HashKeyGet(hPtr);
    ac->func = afunc;
    Jsi_IncrRefCount(interp, afunc);
    if (!Jsi_ValueIsNull(interp, aargs)) {
        ac->args = aargs;
        Jsi_IncrRefCount(interp, aargs);
    }
    ac->intobj = udf;
    ac->dinterp = interp;
    ac->subinterp = sinterp;
    Jsi_HashValueSet(hPtr, ac);
    if (!isthrd)
        return jsi_AliasCreateCmd(sinterp, key, ac);

    Jsi_Value *vasync = Jsi_ValueArrayIndex(interp, args, 3);
    bool async = 0;
    if (vasync && Jsi_GetBoolFromValue(interp, vasync, &async))
        return JSI_ERROR;
        
    if (!async) {
        if (Jsi_MutexLock(interp, sinterp->Mutex) != JSI_OK)
            return JSI_ERROR;
        Jsi_RC rc = jsi_AliasCreateCmd(sinterp, key, ac);
        Jsi_MutexUnlock(interp, sinterp->Mutex);
        return rc;
    }

    /* Post to thread event in sub-interps queue. */
    if (Jsi_MutexLock(interp, sinterp->QMutex) != JSI_OK)
        return JSI_ERROR;

    /* Is an async call. */
    InterpStrEvent *se, *s = (InterpStrEvent *)Jsi_Calloc(1, sizeof(*s));
    // TODO: is s->data inited?
    s->acdata = ac;
    Jsi_DSInit(&s->func);
    Jsi_DSAppend(&s->func, ac->cmdName, NULL);
    se = sinterp->interpStrEvents;
    if (!se)
        sinterp->interpStrEvents = s;
    else {
        while (se->next)
            se = se->next;
        se->next = s;
    }

    Jsi_MutexUnlock(interp, sinterp->QMutex);
    return JSI_OK;
}