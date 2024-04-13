static Jsi_RC StringLastIndexOfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    int sLen, bLen;
    const char *v;
    ChkString(_this, funcPtr, v, &sLen, &bLen);

    Jsi_Value *seq = Jsi_ValueArrayIndex(interp, args, skip);
    Jsi_Value *start = Jsi_ValueArrayIndex(interp, args, 1+skip);

    if (!seq) {
        Jsi_ValueMakeNumber(interp, ret, -1);
        return JSI_OK;
    }

    const char *vseq = Jsi_ValueToString(interp, seq, NULL);
    int istart = 0;
    if (start) {
        if (Jsi_GetIntFromValue(interp, start, &istart)) {
            return JSI_ERROR;
        }
        if (istart < 0) istart = 0;
    }

    int r = Jsi_Strrpos(v, istart, vseq, 0);
    Jsi_ValueMakeNumber(interp, ret, r);

    return JSI_OK;;
}