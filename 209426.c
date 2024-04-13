static Jsi_RC jsi_InterpDebugHook(struct Jsi_Interp* interp, const char *curFile,
    int curLine, int curLevel, const char *curFunc, const char *opCode, jsi_OpCode *op, const char *emsg)
{
    // TODO: when code is run in debugger, parser.y should attribute op for case stmt to skip str compares, etc.
    int isfun=0;
    if (interp->isInCallback || curLine<=0)
        return JSI_OK;
    if (op && op->nodebug)
        return JSI_OK;
    int isbp = 0, bpId = 0, cont = interp->debugOpts.doContinue,
        stop = (interp->debugOpts.noFilter || interp->debugOpts.forceBreak);
    if (!curFunc)
        curFunc = "";

    if (interp->parent && interp->parent->sigmask) {
        interp->parent->sigmask = 0;
        opCode = "SIGINT";

    } else if (Jsi_Strcmp(opCode, "DEBUG") || !interp->parent) {

        // Avoid overhead of multiple ops on same line of code.
        int sameLine = (interp->debugOpts.lastLine == curLine && interp->debugOpts.lastLevel == curLevel
            && interp->debugOpts.lastFile == curFile);

        if (sameLine && stop==0 && (interp->debugOpts.bpLast==0
            || (interp->debugOpts.bpOpCnt+10) >= interp->opCnt)) //TODO: need better way to detect bp dups.
            goto done;

        if (!interp->debugOpts.debugCallback || !interp->parent) {
            fprintf(stderr, "FILE %s:%d (%d) %s %s\n", curFile, curLine, curLevel, curFunc, opCode);
            return JSI_OK;
        }

        // Check for breakpoints.
        if (interp->breakpointHash) {
            Jsi_HashEntry *hPtr;
            Jsi_HashSearch search;
            for (hPtr = Jsi_HashSearchFirst(interp->breakpointHash, &search);
                hPtr != NULL && stop == 0; hPtr = Jsi_HashSearchNext(&search)) {
                jsi_BreakPoint* bptr = (jsi_BreakPoint*)Jsi_HashValueGet(hPtr);
                if (bptr == NULL || bptr->enabled == 0) continue;
                if (bptr->func)
                    stop = (!Jsi_Strcmp(bptr->func, curFunc));
                else
                    stop = (bptr->line == curLine && !Jsi_Strcmp(bptr->file, curFile));
                if (stop) {
                    isbp = 1;
                    bpId = bptr->id;
                    bptr->hits++;
                    if (bptr->temp)
                        bptr->enabled = 0;
                }
            }
        }

        if (stop == 0) { // No breakpoint.
            if (cont  // Cmd is "continue"
                // Handle "next" by skipping calls into functions.
                || (interp->debugOpts.minLevel>0 && curLevel>interp->debugOpts.minLevel)
                || (isfun=(Jsi_Strcmp(opCode, "PUSHVAR")==0 && op[1].op == OP_PUSHFUN)))
            {
                if (isfun) {
                    interp->debugOpts.lastLine = curLine;
                    interp->debugOpts.lastLevel = curLevel;
                    interp->debugOpts.lastFile = curFile;
                }
done:
                return JSI_OK;
            }
        }
    }
    interp->debugOpts.bpLast = isbp;
    interp->debugOpts.bpOpCnt = interp->opCnt;
    interp->debugOpts.lastLine = curLine;
    interp->debugOpts.lastLevel = curLevel;
    interp->debugOpts.lastFile = curFile;
    interp->debugOpts.forceBreak = 0;

    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    if (emsg && Jsi_Strchr(emsg,'\"'))
        emsg = 0;
    Jsi_DSPrintf(&dStr, "[\"%s\", %d, %d, \"%s\", \"%s\", %d, \"%s\"]", curFile?curFile:"", curLine, curLevel, curFunc, opCode, bpId, emsg?emsg:"");
    interp->isInCallback = 1;
    Jsi_RC rc = JSI_ERROR;
    if (interp->debugOpts.debugCallback)
        rc = Jsi_FunctionInvokeJSON(interp->parent, interp->debugOpts.debugCallback, Jsi_DSValue(&dStr), &interp->retValue);
    interp->isInCallback = 0;
    if (interp->parent->exited == 0 && rc != JSI_OK)
        Jsi_LogError("debugger failure");
    return rc;
}