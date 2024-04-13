void jsi_TraceFuncCall(Jsi_Interp *interp, Jsi_Func *fstatic, jsi_OpCode *iPtr,
    Jsi_Value *_this, Jsi_Value* args, Jsi_Value *ret, int tc)
{
    jsi_OpCode *ip = (iPtr ? iPtr : interp->curIp);
    if (!ip)
        return;
    const char *ff, *fname = ip->fname?ip->fname:"";
    if ((tc&jsi_callTraceFullPath)==0 && ((ff=Jsi_Strrchr(fname,'/'))))
        fname = ff+1;
    if (interp->traceHook)
        (*interp->traceHook)(interp, fstatic->name, ip->fname, ip->Line, fstatic->cmdSpec, _this, args, ret);
    else {
        const char *fp = ((tc&jsi_callTraceNoParent)?NULL:fstatic->parentName);
        if (fp && !*fp)
            fp = NULL;
        Jsi_DString aStr;
        Jsi_DSInit(&aStr);
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        Jsi_DString pStr;
        Jsi_DSInit(&pStr);
        Jsi_DString *sPtr = NULL;
        int plen = 0;
        if (ret) {
            sPtr = &dStr;
            Jsi_DSAppend(sPtr, " <-- ", NULL);
            plen = Jsi_DSLength(sPtr);
            Jsi_ValueGetDString(interp, ret, sPtr, 0);
        } else if ((tc&jsi_callTraceArgs)) {
            sPtr = &aStr;
            Jsi_ValueGetDString(interp, args, sPtr, JSI_OUTPUT_JSON);
        }
        if (sPtr) {
            if (!(tc&jsi_callTraceNoTrunc)) {
                const char *cp0 = Jsi_DSValue(sPtr), *cp1 = Jsi_Strchr(cp0, '\n');
                int nlen = 0, clen = Jsi_DSLength(sPtr);
                if (cp1) {
                    nlen = (cp1-cp0);
                    if (nlen>60) nlen = 60;
                }  else if (clen>60)
                    nlen = 60;
                else nlen = clen;
                if (nlen != clen && clen>plen) {
                    Jsi_DSSetLength(sPtr, nlen);
                    Jsi_DSAppend(sPtr, "...", NULL);
                }
            }
        }
        if (interp->parent && interp->debugOpts.traceCallback) {
            Jsi_DString jStr={}, kStr={}, lStr={};
            Jsi_DSPrintf(&kStr, "[\"%s%s%s\", %s, %s, \"%s\", %d, %d ]",
                (fp?fp:""), (fp?".":""), fstatic->name, 
                (ret?"null":Jsi_JSONQuote(interp, Jsi_DSValue(&aStr),-1, &jStr)),
                (ret?Jsi_JSONQuote(interp, Jsi_DSValue(&dStr),-1, &lStr):"null"),
                 fname, ip->Line, ip->Lofs);
            if (Jsi_FunctionInvokeJSON(interp->parent, interp->debugOpts.traceCallback, Jsi_DSValue(&kStr), NULL) != JSI_OK)
                Jsi_DSPrintf(&pStr, "failed trace call\n");
            Jsi_DSFree(&jStr);
            Jsi_DSFree(&kStr);
            Jsi_DSFree(&lStr);
        } else if ((tc&jsi_callTraceBefore))
            Jsi_DSPrintf(&pStr, "%s:%d %*s#%d: %c %s%s%s(%s) %s\n",
                fname, ip->Line,
                (interp->level-1)*2, "", interp->level,
                (ret?'<':'>'), (fp?fp:""), (fp?".":""), fstatic->name, Jsi_DSValue(&aStr), Jsi_DSValue(&dStr));
        else {
            if (!interp->curIp || !interp->logOpts.before) {
                Jsi_DSPrintf(&pStr, "%*s#%d: %c %s%s%s(%s) in %s:%d%s\n", (interp->level-1)*2, "", interp->level,
                    (ret?'<':'>'), (fp?fp:""), (fp?".":""), fstatic->name, Jsi_DSValue(&aStr),
                fname, ip->Line, Jsi_DSValue(&dStr));
            } else {
                int quote = 0;
                jsi_SysPutsCmdPrefix(interp, &interp->logOpts, &pStr, &quote, NULL);
                Jsi_DSPrintf(&pStr, "%*s#%d: %c %s%s%s(%s) \n", (interp->level-1)*2, "", interp->level,
                    (ret?'<':'>'), (fp?fp:""), (fp?".":""), fstatic->name, Jsi_DSValue(&aStr));
                Jsi_DSAppend(&pStr, Jsi_DSValue(&dStr), NULL);
            }
        }
        if (Jsi_DSLength(&pStr))
            Jsi_Puts(interp, jsi_Stderr, Jsi_DSValue(&pStr), -1);
        Jsi_DSFree(&pStr);
        Jsi_DSFree(&dStr);
        Jsi_DSFree(&aStr);
    }
}