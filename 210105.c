int Jsi_Puts(Jsi_Interp *interp, Jsi_Channel chan, const char *str, int size)
{
    int code = 0, len = Jsi_Strlen(str);
    if (chan->fsPtr==0 || !chan->fsPtr->putsProc) {
        FILE *fp = (chan && chan->fp ? chan->fp : stdout);
        code = fputs(str, fp);
    } else {
        if (interp && interp->debugOpts.putsCallback && interp->parent) {
            int code = 0;
            Jsi_DString jStr={}, kStr={};
            Jsi_DSPrintf(&kStr, "[%s, %d]",
                Jsi_JSONQuote(interp, str, size, &jStr), (chan->fp == stderr?1:0));
            if (Jsi_FunctionInvokeJSON(interp->parent, interp->debugOpts.putsCallback, Jsi_DSValue(&kStr), NULL) != JSI_OK)
                code = 1;
            Jsi_DSFree(&jStr);
            Jsi_DSFree(&kStr);
            return code;
        } else if (interp && interp->stdoutStr) {
            Jsi_DString dStr = {};
            Jsi_DSAppend(&dStr, Jsi_ValueString(interp, interp->stdoutStr, NULL), NULL);
            Jsi_DSAppend(&dStr, str, NULL);
            Jsi_ValueFromDS(interp, &dStr, &interp->stdoutStr);
        } else

            code = chan->fsPtr->putsProc(chan, str);
    }
    if (size>=0 && len < size) {
        Jsi_Puts(interp, chan, "\\0", -1);
        Jsi_Puts(interp, chan, str+len+1, size-len-1);
    }
    return code;
}