static Jsi_RC jsiInterpDelete(Jsi_Interp* interp, void *unused)
{
    SIGASSERT(interp,INTERP);
    bool isMainInt = (interp == jsiIntData.mainInterp);
    int mainFlag = (isMainInt ? 2 : 1);
    if (isMainInt)
        DeleteAllInterps();
    if (interp->opts.initProc)
        (*interp->opts.initProc)(interp, mainFlag);
    jsiIntData.delInterp = interp;
    if (interp->gsc) jsi_ScopeChainFree(interp, interp->gsc);
    if (interp->csc) Jsi_DecrRefCount(interp, interp->csc);
    if (interp->ps) jsi_PstateFree(interp->ps);
    int i;
    for (i=0; i<interp->maxStack; i++) {
        if (interp->Stack[i]) Jsi_DecrRefCount(interp, interp->Stack[i]);
        if (interp->Obj_this[i]) Jsi_DecrRefCount(interp, interp->Obj_this[i]);
    }
    if (interp->Stack) {
        Jsi_Free(interp->Stack);
        Jsi_Free(interp->Obj_this);
    }

    if (interp->argv0)
        Jsi_DecrRefCount(interp, interp->argv0);
    if (interp->console)
        Jsi_DecrRefCount(interp, interp->console);
    if (interp->lastSubscriptFail)
        Jsi_DecrRefCount(interp, interp->lastSubscriptFail);
    if (interp->nullFuncRet)
        Jsi_DecrRefCount(interp, interp->nullFuncRet);
    Jsi_HashDelete(interp->codeTbl);
    Jsi_MapDelete(interp->cmdSpecTbl);
    Jsi_HashDelete(interp->fileTbl);
    Jsi_HashDelete(interp->funcObjTbl);
    Jsi_HashDelete(interp->funcsTbl);
    if (interp->profileCnt) { // TODO: resolve some values from dbPtr, others not.
        double endTime = jsi_GetTimestamp();
        double coverage = (int)(100.0*interp->coverHit/interp->coverAll);
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        Jsi_DSPrintf(&dStr, "PROFILE: TOTAL: time=%.6f, func=%.6f, cmd=%.6f, #funcs=%d, #cmds=%d, cover=%2.1f%%, #values=%d, #objs=%d %s%s\n",
            endTime-interp->startTime, interp->funcSelfTime, interp->cmdSelfTime, interp->funcCallCnt, interp->cmdCallCnt,
            coverage, interp->dbPtr->valueAllocCnt,  interp->dbPtr->objAllocCnt,
            interp->parent?" ::":"", (interp->parent&&interp->name?interp->name:""));
        Jsi_Puts(interp, jsi_Stderr, Jsi_DSValue(&dStr), -1);
        Jsi_DSFree(&dStr);
    }
    if (isMainInt)
        Jsi_HashDelete(interp->lexkeyTbl);
    Jsi_HashDelete(interp->protoTbl);
    if (interp->subthread)
        jsiIntData.mainInterp->threadCnt--;
    if (interp->subthread && interp->strKeyTbl == jsiIntData.mainInterp->strKeyTbl)
        jsiIntData.mainInterp->threadShrCnt--;
    if (!jsiIntData.mainInterp->threadShrCnt)
#ifdef JSI_USE_MANY_STRKEY
        jsiIntData.mainInterp->strKeyTbl->v.tree->opts.lockTreeProc = NULL;
#else
        jsiIntData.mainInterp->strKeyTbl->v.hash->opts.lockHashProc = NULL;
#endif
    //Jsi_ValueMakeUndef(interp, &interp->ret);
    Jsi_HashDelete(interp->thisTbl);
    Jsi_HashDelete(interp->varTbl);
    Jsi_HashDelete(interp->genValueTbl);
    Jsi_HashDelete(interp->genObjTbl);
    Jsi_HashDelete(interp->aliasHash);
    if (interp->staticFuncsTbl)
        Jsi_HashDelete(interp->staticFuncsTbl);
    if (interp->breakpointHash)
        Jsi_HashDelete(interp->breakpointHash);
    if (interp->preserveTbl->numEntries!=0)
        Jsi_LogBug("Preserves unbalanced");
    Jsi_HashDelete(interp->preserveTbl);
    if (interp->curDir)
        Jsi_Free(interp->curDir);
    if (isMainInt) {
        jsi_InitFilesys(interp, mainFlag);
    }
#ifndef JSI_OMIT_VFS
    jsi_InitVfs(interp, 1);
#endif
#ifndef JSI_OMIT_CDATA
        jsi_InitCData(interp, mainFlag);
#endif
#if JSI__MYSQL==1
        Jsi_InitMySql(interp, mainFlag);
#endif
    while (interp->interpStrEvents) {
        InterpStrEvent *se = interp->interpStrEvents;
        interp->interpStrEvents = se->next;
        if (se->acfunc)
            Jsi_DecrRefCount(interp, se->acfunc);
        if (se->acdata)
            Jsi_Free(se->acdata);
        Jsi_Free(se);
    }

    if (interp->Mutex)
        Jsi_MutexDelete(interp, interp->Mutex);
    if (interp->QMutex) {
        Jsi_MutexDelete(interp, interp->QMutex);
        Jsi_DSFree(&interp->interpEvalQ);
    }
    if (interp->nullFuncArg)
        Jsi_DecrRefCount(interp, interp->nullFuncArg);
    if (interp->NullValue)
        Jsi_DecrRefCount(interp, interp->NullValue);
    if (interp->Function_prototype_prototype) {
        if (interp->Function_prototype_prototype->refCnt>1)
            Jsi_DecrRefCount(interp, interp->Function_prototype_prototype);
        Jsi_DecrRefCount(interp, interp->Function_prototype_prototype);
    }
    if (interp->Object_prototype) {
        Jsi_DecrRefCount(interp, interp->Object_prototype);
    }
    Jsi_HashDelete(interp->regexpTbl);
    Jsi_OptionsFree(interp, InterpOptions, interp, 0);
    Jsi_HashDelete(interp->userdataTbl);
    Jsi_HashDelete(interp->eventTbl);
    if (interp->inopts)
        Jsi_DecrRefCount(interp, interp->inopts);
    if (interp->safeWriteDirs)
        Jsi_DecrRefCount(interp, interp->safeWriteDirs);
    if (interp->safeReadDirs)
        Jsi_DecrRefCount(interp, interp->safeReadDirs);
    if (interp->pkgDirs)
        Jsi_DecrRefCount(interp, interp->pkgDirs);
    for (i=0; interp->cleanObjs[i]; i++) {
        interp->cleanObjs[i]->tree->opts.freeHashProc = 0;
        Jsi_ObjFree(interp, interp->cleanObjs[i]);
    }
    Jsi_HashDelete(interp->bindTbl);
    for (i = 0; i <= interp->cur_scope; i++)
        jsi_ScopeStrsFree(interp, interp->scopes[i]);
#if JSI__ZVFS==1
    Jsi_InitZvfs(interp, mainFlag);
#endif
    if (!interp->parent)
        Jsi_HashDelete(interp->loadTbl);
    if (interp->packageHash)
        Jsi_HashDelete(interp->packageHash);
    Jsi_HashDelete(interp->assocTbl);
    interp->cleanup = 1;
    jsi_AllObjOp(interp, NULL, -1);
#ifdef JSI_MEM_DEBUG
    jsi_DebugDumpValues(interp);
#endif
    if (isMainInt || interp->strKeyTbl != jsiIntData.mainInterp->strKeyTbl)
        Jsi_MapDelete(interp->strKeyTbl);

    if (isMainInt)
        jsiIntData.mainInterp = NULL;
    _JSI_MEMCLEAR(interp);
    jsiIntData.delInterp = NULL;
    Jsi_Free(interp);
    return JSI_OK;
}