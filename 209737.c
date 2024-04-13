Jsi_RC jsi_InterpInfo(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Interp *sinterp = interp;
    Jsi_DString dStr = {}, cStr = {};
    char tbuf[1024];
    Jsi_Value *v = Jsi_ValueArrayIndex(interp, args, 0);
    tbuf[0] = 0;
    if (v) {
        InterpObj *udf = NULL;
        if (v && v->vt == JSI_VT_OBJECT && v->d.obj->ot == JSI_OT_USEROBJ)
            udf = (InterpObj *)v->d.obj->d.uobj->data;
        if (udf && udf->subinterp) {
            SIGASSERT(udf, INTERPOBJ);
            sinterp = udf->subinterp;
        } else
            return Jsi_LogError("unknown interp");
    }
    if (sinterp->subthread)
        snprintf(tbuf, sizeof(tbuf), ", thread:{errorCnt:%u, evalCnt:%u, msgCnt:%u }",
            sinterp->threadErrCnt, sinterp->threadEvalCnt, sinterp->threadMsgCnt );
    const char *funcstr = sinterp->framePtr->funcName;
    if (!funcstr)
        funcstr = "";
    int curLine = (sinterp->curIp?sinterp->curIp->Line:0);
    Jsi_DSPrintf(&dStr, "{curLevel:%d, curLine:%d, curFile:\"%s\", curFunc:\"%s\", hasExited:%d, "
        "opCnt:%d, isSafe:%s, depth:%d, codeCacheHits: %d, typeMismatchCnt:%d, "
        "funcCallCnt:%d, cmdCallCnt:%d, includeCnt:%d, includeDepth:%d, pkgReqDepth:%d, "
        "cwd:\"%s\", lockTimeout: %d, name, \"%s\" %s%s};",
        sinterp->level, curLine, jsi_GetCurFile(sinterp), funcstr?funcstr:"",
        sinterp->exited, sinterp->opCnt, sinterp->isSafe?"true":"false",
        sinterp->interpDepth, sinterp->codeCacheHit, sinterp->typeMismatchCnt,
        sinterp->funcCallCnt, sinterp->cmdCallCnt, sinterp->includeCnt, sinterp->includeDepth, sinterp->pkgReqDepth,
        (sinterp->curDir?sinterp->curDir:Jsi_GetCwd(sinterp,&cStr)),
        sinterp->lockTimeout, sinterp->name?sinterp->name:"", tbuf[0]?",":"", tbuf);
    Jsi_RC rc = Jsi_JSONParse(interp, Jsi_DSValue(&dStr), ret, 0);
    if (rc != JSI_OK)
        puts(Jsi_DSValue(&dStr));
    Jsi_DSFree(&dStr);
    Jsi_DSFree(&cStr);
    return rc;
}