static Jsi_OpCodes *code_fcall(jsi_Pstate *p, jsi_Pline *line, int argc, const char *name, const char *namePre, Jsi_OpCodes *argCodes, Jsi_OpCodes* pref) {
    Jsi_OpCodes *codes = code__fcall(p, line, argc, name, namePre, argCodes);
    int i;
    if (!name || !codes || !pref)
        return codes;
    jsi_OpLogFlags logflag = jsi_Oplf_none;
    if (name[0] == 'a' && !Jsi_Strcmp(name, "assert"))
        logflag = jsi_Oplf_assert;
    else if (name[0] == 'L' && name[1] == 'o') {
        if (!Jsi_Strcmp(name, "LogDebug"))
            logflag = jsi_Oplf_debug;
        else if (!Jsi_Strcmp(name, "LogTrace"))
            logflag = jsi_Oplf_trace;
        else if (!Jsi_Strcmp(name, "LogTest"))
            logflag = jsi_Oplf_test;
    }
    if (logflag) {
        codes->codes[0].logflag = logflag;
        if (argCodes)
            for (i=0; i<argCodes->code_len; i++)
                argCodes->codes[i].logflag = logflag;
        for (i=0; i<pref->code_len; i++)
            pref->codes[i].logflag = logflag;
    }
    return codes;
}