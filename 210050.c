static Jsi_RC FilesysReadCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int  sum = 0, n;
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    UdfGet(udf, _this, funcPtr);
    char buf[JSI_BUFSIZ];
    int argc = Jsi_ValueGetLength(interp, args);
    int nsiz = -1, cnt = 0, rsiz;
    
    if (!udf->filename) {
        goto bail;
    }
    if (argc>0 && Jsi_GetIntFromValue(interp, Jsi_ValueArrayIndex(interp, args, 0), &nsiz) != JSI_OK)
        goto bail;
    while (cnt++ < MAX_LOOP_COUNT) {
        rsiz = sizeof(buf);
        if (nsiz>0 && ((sum+rsiz)>nsiz))
            rsiz = (nsiz-sum);
        if ((n = Jsi_Read(interp, udf->chan, buf, rsiz)) <= 0)
            break;
        Jsi_DSAppendLen(&dStr, buf, n);
        sum += n;
        if (nsiz>=0 && sum >=nsiz)
            break;
    }
    if (Jsi_DSLength(&dStr)>0)
        Jsi_ValueMakeDStringObject(interp, ret, &dStr);
    return JSI_OK;
    
bail:
    Jsi_DSFree(&dStr);
    Jsi_ValueMakeUndef(interp, ret);
    return JSI_OK;
}