static Jsi_RC _StringTrimCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this, // TODO: UTF
    Jsi_Value **ret, Jsi_Func *funcPtr, int ends)
{
    const char *tstr = " \t\n\r", *vstr;
    int vend,  n, tlen = Jsi_Strlen(tstr), clen, bLen;
    ChkString(_this, funcPtr, vstr, &clen, &bLen);
    
    Jsi_Value *tchars = Jsi_ValueArrayIndex(interp, args, skip);
    
    if (tchars) {
        tstr = Jsi_ValueToString(interp, tchars, NULL);
        tlen = Jsi_Strlen(tstr);
    }
    
    if (ends&1) {
        while (*vstr) {
            for (n=0; n<tlen; n++)
                if (tstr[n] == *vstr) break;
            if (n>=tlen) break;
            vstr++;
            clen--;
        }
    }
    vend = clen-1;
    if (ends&2) {
        for (; vend>=0; vend--) {
            for (n=0; n<tlen; n++)
                if (tstr[n] == vstr[vend]) break;
            if (n>=tlen) break;
        }
    }
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_UtfSubstr(vstr, 0, vend+1, &dStr);
    Jsi_ValueFromDS(interp, &dStr, ret);
    return JSI_OK;
}