Jsi_RC jsi_FunctionSubCall(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Value *tocall, int discard)
{
    Jsi_RC rc = JSI_OK;
    const char *oldCurFunc = interp->curFunction, *spnam = "";
    jsi_OpCode *ip = interp->curIp;
    int adds, as_constructor = (ip->op == OP_NEWFCALL);
    double timStart = 0;
    int docall;
    int calltrc = 0, profile = interp->profile, coverage = interp->coverage;
    jsi_PkgInfo *pkg;
    int tc;
    
    //char *lpv = interp->lastPushStr;
    if (tocall->vt == JSI_VT_UNDEF && tocall->f.bits.lookupfailed && tocall->d.lookupFail && !interp->noAutoLoad) {
        spnam = tocall->d.lookupFail;
        tocall->f.bits.lookupfailed = 0;
        tocall = jsi_LoadFunction(interp, spnam, tocall);
        interp->lastPushStr = (char*)spnam;
        interp->curIp = ip;
    }
    if (!Jsi_ValueIsFunction(interp, tocall)) {
       // if (tocall->f.bits.subscriptfailed && tocall->d.lookupFail)
       //     spnam = tocall->d.lookupFail;
        jsi_DumpFunctions(interp, spnam);
        rc = JSI_ERROR;
        goto empty_func;
    }

    if (tocall->d.obj->d.fobj==NULL || tocall->d.obj->d.fobj->func==NULL) {   /* empty function */
empty_func:
        //jsiPop(interp, stackargc);
        //jsiClearStack(interp,1);
        //Jsi_ValueMakeUndef(interp, &_jsi_TOP);
        interp->curFunction = oldCurFunc;
        Jsi_DecrRefCount(interp, _this);
        if (rc==JSI_OK)
            rc = JSI_CONTINUE;
        return rc;
        //goto done;
    }
    
    Jsi_FuncObj *fobj = tocall->d.obj->d.fobj;
    Jsi_Func *funcPtr = fobj->func;
    if (funcPtr->callback == jsi_NoOpCmd || tocall->d.obj->isNoOp) {
        jsi_NoOpCmd(interp, NULL, NULL, NULL, NULL);
        goto empty_func;
    }
    if (!interp->asserts && funcPtr->callback == jsi_AssertCmd)
        goto empty_func;
    const char *onam = funcPtr->name;
//        if (!onam) // Override blank name with last index.
//            funcPtr->name = lpv;
    if (funcPtr->name && funcPtr->name[0] && funcPtr->type == FC_NORMAL)
        interp->curFunction = funcPtr->name;
    adds = funcPtr->callflags.bits.addargs;
    Jsi_CmdSpec *cs  = funcPtr->cmdSpec;
    if (adds && (cs->flags&JSI_CMDSPEC_NONTHIS))
        adds = 0;

    Jsi_Func *pprevActive = interp->prevActiveFunc, *prevActive = interp->prevActiveFunc = interp->activeFunc;
    interp->activeFunc = funcPtr;

    rc = jsi_SharedArgs(interp, args, funcPtr, 1); /* make arg vars to share arguments */
    if (rc != JSI_OK)
        goto bail;
    funcPtr->callflags.bits.addargs = 0;
    jsi_InitLocalVar(interp, args, funcPtr);
    jsi_SetCallee(interp, args, tocall);
    
    pkg = funcPtr->pkg;
    tc = interp->traceCall;
    if (pkg) {
        tc |= pkg->popts.modConf.traceCall;
        profile |= pkg->popts.modConf.profile;
        coverage |= pkg->popts.modConf.coverage;
    }

    if (as_constructor) {                       /* new Constructor */
        Jsi_Obj *newobj = Jsi_ObjNewType(interp, JSI_OT_OBJECT);
        Jsi_Value *proto = NULL;
        if (!interp->subOpts.noproto)
            proto = Jsi_ValueObjLookup(interp, tocall, "prototype", 0);
        if (proto && proto->vt == JSI_VT_OBJECT) {
            newobj->__proto__ = proto;
            newobj->clearProto = 1;
            Jsi_IncrRefCount(interp, proto);
        }
        Jsi_ValueReset(interp, &_this);
        Jsi_ValueMakeObject(interp, &_this, newobj);            
        /* TODO: constructor specifics??? */
        calltrc = (tc&jsi_callTraceNew);
    }
    if (funcPtr->type == FC_NORMAL)
        calltrc = (tc&jsi_callTraceFuncs);
    else
        calltrc = (tc&jsi_callTraceCmds);
    if (calltrc && funcPtr->name)
        jsi_TraceFuncCall(interp, funcPtr, ip, _this, args, 0, tc);

    //Jsi_Value *spretPtr = *ret;
 
    interp->activeFunc = funcPtr;
    docall = (rc==JSI_OK);
    if (profile || coverage) {
        interp->profileCnt++;
        timStart = jsi_GetTimestamp();
    }
    if (funcPtr->type == FC_NORMAL) {
        if (docall) {
            rc = jsi_evalcode(interp->ps, funcPtr, funcPtr->opcodes, tocall->d.obj->d.fobj->scope, 
                args, _this, ret);
        }
        interp->funcCallCnt++;
    } else if (!funcPtr->callback) {
        Jsi_LogError("can not call:\"%s()\"", funcPtr->name);
    } else {
        int oldcf = funcPtr->callflags.i;
        funcPtr->callflags.bits.iscons = (as_constructor?JSI_CALL_CONSTRUCTOR:0);
        if (funcPtr->f.bits.hasattr)
        {
#define SPTR(s) (s?s:"")
            if ((funcPtr->f.bits.isobj) && _this->vt != JSI_VT_OBJECT) {
                rc = JSI_ERROR;
                docall = 0;
                Jsi_LogError("'this' is not object: \"%s()\"", funcPtr->name);
            } else if ((!(funcPtr->f.bits.iscons)) && as_constructor) {
                rc = JSI_ERROR;
                docall = 0;
                Jsi_LogError("can not call as constructor: \"%s()\"", funcPtr->name);
            } else {
                int aCnt = Jsi_ValueGetLength(interp, args);
                if (aCnt<(cs->minArgs+adds)) {
                    Jsi_LogError("missing args, expected \"%s(%s)\" ", cs->name, SPTR(cs->argStr));
                    rc = JSI_ERROR;
                    docall = 0;
                } else if (cs->maxArgs>=0 && (aCnt>cs->maxArgs+adds)) {
                    Jsi_LogError("extra args, expected \"%s(%s)\" ", cs->name, SPTR(cs->argStr));
                    rc = JSI_ERROR;
                    docall = 0;
                }
            }
        }
        if (docall) {
            funcPtr->fobj = fobj; // Backlink for bind.
            funcPtr->callflags.bits.isdiscard = discard;
            rc = funcPtr->callback(interp, args, 
                _this, ret, funcPtr);
            interp->cmdCallCnt++;
        }
        funcPtr->callflags.i = oldcf;
    }
    if (profile || coverage) {
        double timEnd = jsi_GetTimestamp(), timUsed = (timEnd - timStart);;
        assert(timUsed>=0);
        funcPtr->allTime += timUsed;
        if (interp->framePtr->evalFuncPtr)
            interp->framePtr->evalFuncPtr->subTime += timUsed;
        else
            interp->subTime += timUsed;
    }
    if (calltrc && (tc&jsi_callTraceReturn) && funcPtr->name)
        jsi_TraceFuncCall(interp, funcPtr, ip, _this, NULL, *ret, tc);
    if (!onam)
        funcPtr->name = NULL;
    if (docall) {
        funcPtr->callCnt++;
        if (rc == JSI_OK && !as_constructor && funcPtr->retType && (interp->typeCheck.all || interp->typeCheck.run))
            rc = jsi_ArgTypeCheck(interp, funcPtr->retType, *ret, "returned from", funcPtr->name, 0, funcPtr, 0);
    }
    interp->prevActiveFunc = pprevActive;
    interp->activeFunc = prevActive;

    if (as_constructor) {
        if (_this->vt == JSI_VT_OBJECT)
            _this->d.obj->constructor = tocall->d.obj;
        if ((*ret)->vt != JSI_VT_OBJECT) {
            Jsi_ValueReset(interp, ret);
            Jsi_ValueCopy(interp, *ret, _this);
        }
    }

bail:
    jsi_SharedArgs(interp, args, funcPtr, 0); /* make arg vars to shared arguments */
    Jsi_DecrRefCount(interp, _this);
    interp->curFunction = oldCurFunc;

    return rc;
}