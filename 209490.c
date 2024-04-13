Jsi_RC jsi_ParseTypeCheckStr(Jsi_Interp *interp, const char *str) {
    uint *iptr = (uint*)&interp->typeCheck;
    const char *wcp = str, *wcn = wcp;
    while (wcn && wcp) {
        int isnot = 0;
        if (*wcp == '!') { isnot = 1; wcp++; }
        wcn = Jsi_Strchr(wcp, ',');
        int ti, wlen = (wcn?(wcn-wcp):(int)Jsi_Strlen(wcp));
#define _JSIPARSETYPES(nam, field) \
        if (wlen == (sizeof(#nam)-1) && !Jsi_Strncmp(#nam, wcp, (sizeof(#nam)-1))) { \
            interp->field = (1-isnot); \
            wcp = (wcn?wcn+1:NULL); \
            continue; \
        }
        _JSIPARSETYPES(Debug, logOpts.Debug)
        _JSIPARSETYPES(Trace, logOpts.Trace)
        _JSIPARSETYPES(Test,  logOpts.Test)
        _JSIPARSETYPES(Info, logOpts.Info)
        _JSIPARSETYPES(Warn, logOpts.Warn)
        _JSIPARSETYPES(Error,  logOpts.Error)
        _JSIPARSETYPES(full,  logOpts.full)
        _JSIPARSETYPES(before,  logOpts.before)
        _JSIPARSETYPES(time,  logOpts.time)
        _JSIPARSETYPES(date,  logOpts.date)
        _JSIPARSETYPES(asserts, asserts)
        _JSIPARSETYPES(assert, asserts)
        _JSIPARSETYPES(noproto, subOpts.noproto)

        const char **tstrs = jsi_TypeChkStrs;
        for (ti=0; tstrs[ti]; ti++) {
            wlen = Jsi_Strlen(tstrs[ti]);
            if (!Jsi_Strncmp(tstrs[ti], wcp, wlen) && (!tstrs[ti][wlen] || tstrs[ti][wlen] == ',')) break;
        }
        if (tstrs[ti]) {
            if (isnot)
                *iptr &= ~(1<<ti);
            else {
                *iptr |= (1<<ti);
                if (!Jsi_Strcmp(tstrs[ti], "all"))
                    interp->typeCheck.parse = interp->typeCheck.run = 1;
                if (!Jsi_Strcmp(tstrs[ti], "strict")) {
                    interp->typeCheck.parse = interp->typeCheck.run = interp->typeCheck.all = 1;
                    if (interp->framePtr->level<=0 || interp->isMain)
                        interp->strict = 1;
                }
            }
        } else {
            Jsi_DString wStr = {};
            int i;
            tstrs = jsi_TypeChkStrs;
            for (i=0; tstrs[i]; i++) Jsi_DSAppend(&wStr, i?", ":"", tstrs[i], NULL);
            Jsi_LogWarn("unknown typeCheck warn option(s) \"%s\" not in: Debug, Trace, Test, Info, Warn, Error, assert, %s, noproto, full, before, time, date", str, Jsi_DSValue(&wStr));
            Jsi_DSFree(&wStr);
        }
        wcp = (wcn?wcn+1:NULL);
    }
    return JSI_OK;
}