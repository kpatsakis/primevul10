void jsi_FuncFree(Jsi_Interp *interp, Jsi_Func *func)
{
    if (--func->refCnt > 0)
        return;
    jsi_PkgInfo *pkg = func->pkg;
    bool profile = (interp->profile || (pkg?pkg->popts.modConf.profile:0)), 
        cover = (interp->coverage || (pkg?pkg->popts.modConf.coverage:0));
    if (profile || cover) {
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        const char *file = func->script;
        if (!file)
            file = "";
        int line = func->bodyline.last_line;
        if (!func->callCnt) {
            if (cover && func->type == FC_NORMAL && func->name)
                Jsi_DSPrintf(&dStr, "COVERAGE: func=%s  file=%s:%d  cover=0%%\n", func->name, file, line );
        } else {
            char buf[JSI_BUFSIZ];
            if (func->type == FC_BUILDIN) {
                const char *fnam = func->parentName;
                snprintf(buf, sizeof(buf), "cmd=%s%s%s", fnam, fnam[0]?".":"", func->name);
                interp->cmdSelfTime += (func->allTime-func->subTime);
            } else {
                
                double coverage = 0; // Calculate hits/all.
                if (func->opcodes && func->opcodes->code_len>0) {
                    int i, cchit=0, ccall=0, ccline=0, cchitline=0;
                    Jsi_OpCodes *oc = func->opcodes;
                    for (i=0; i<oc->code_len; i++) {
                        if (oc->codes[i].Line<=0) continue;
                        if (ccline != oc->codes[i].Line) {
                            ccline = oc->codes[i].Line;
                            ccall++;
                            interp->coverAll++;
                        }
                        if (cchitline != oc->codes[i].Line && oc->codes[i].hit) {
                            cchitline = oc->codes[i].Line;
                            cchit++;
                            interp->coverHit++;
                        }
                    }
                    if (ccall)
                        coverage = (int)(100.0*cchit/ccall);
                        
                    if (cover && cchit<ccall) { // Generate the misses list.
                        char cbuf[JSI_BUFSIZ];
                        int lastout = 0, lastpos=0, dupcnt=0, cccnt=0;
                        cbuf[0] = 0;
                        ccline=cchitline=0;
                        for (i=0; i<oc->code_len; i++) {
                            int ismiss = 0;
                            if (i==oc->code_len) {
                                ismiss = (ccline>0 && cchitline != ccline);
                            } else {
                                if (oc->codes[i].Line<=0) continue;
                                ismiss = (ccline>0 && ccline != oc->codes[i].Line && cchitline != ccline);
                            }
                            if (ismiss) {
                                cccnt++;
                                const char *sep = (cccnt>1?",":"");
                                if (lastout && (lastout+1)==ccline) {
                                    sep = "-";
                                    dupcnt++;
                                    if (dupcnt>1)
                                        cbuf[lastpos]=0; // Inefficient, but reliable.
                                    else
                                        lastpos = Jsi_Strlen(cbuf);
                                } else 
                                    dupcnt = 0;
                                int cbl = Jsi_Strlen(cbuf);
                                snprintf(cbuf+cbl, sizeof(cbuf)-cbl, "%s%d", sep, ccline);
                                lastout = ccline;
                            }
                            ccline = oc->codes[i].Line;
                            if (oc->codes[i].hit)
                                cchitline = ccline;
                        }
                        Jsi_DSPrintf(&dStr, "COVERAGE: func=%s  file=%s:%d  cover=%2.1f%%  hits=%d,  all=%d,  misses=%s\n", 
                            func->name, file, line, coverage, cchit, ccall, cbuf);
                    }
                }
                snprintf(buf, sizeof(buf), "cover=%#5.1f%%  func=%s file=%s:%d", coverage, func->name, file, line);
                interp->funcSelfTime += (func->allTime-func->subTime);
            }
            if (profile)
                Jsi_DSPrintf(&dStr, "PROFILE:  self=%6.6f  all=%6.6f  #calls=%-8d  self/call=%6.6f  all/call=%6.6f  %s %s%s\n",
                    (func->allTime-func->subTime), (double)(func->allTime), func->callCnt, 
                    (double)(func->allTime-func->subTime)/func->callCnt,  (double)(func->allTime)/func->callCnt, 
                    buf, interp->parent?" ::":"", (interp->parent&&interp->name?interp->name:""));
        }
        if (Jsi_DSLength(&dStr))
            Jsi_Puts(interp, jsi_Stderr, Jsi_DSValue(&dStr), -1);
        Jsi_DSFree(&dStr);
    }

    if (func->opcodes)
        jsi_FreeOpcodes(func->opcodes);
    if (func->hPtr)
        Jsi_HashEntryDelete(func->hPtr);
    if (func->localnames)
        jsi_ScopeStrsFree(interp, func->localnames);
    if (func->argnames)
        jsi_ScopeStrsFree(interp, func->argnames);
    if (func->cmdSpec && (intptr_t)func->cmdSpec->reserved[3]& 0x1)
        Jsi_Free(func->cmdSpec);
    _JSI_MEMCLEAR(func);
    Jsi_Free(func);
    interp->funcCnt--;
}