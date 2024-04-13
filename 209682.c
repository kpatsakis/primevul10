Jsi_Value *Jsi_NameLookup(Jsi_Interp *interp, const char *name)
{
    uint cnt = 0, len, isq;
    char *nam = (char*)name, *cp, *cp2, *ocp, *kstr;
    //DECL_VALINIT(tv);
    DECL_VALINIT(nv);
    DECL_VALINIT(key);
    Jsi_Value *v = NULL, *nvPtr = &nv;
    Jsi_Value *kPtr = &key; // Note: a string key so no reset needed.
    Jsi_DString dStr = {};
    cp2 = Jsi_Strchr(nam,'[');
    cp = Jsi_Strchr(nam, '.');
    if (cp2 && (cp==0 || cp2<cp))
        cp = cp2;
    if (!cp)
        return Jsi_VarLookup(interp, nam);
    //fprintf(stderr, "NAM: %s\n", nam);
    Jsi_DSSetLength(&dStr, 0);
    Jsi_DSAppendLen(&dStr, nam, cp-nam);
    v = Jsi_VarLookup(interp, Jsi_DSValue(&dStr));
    if (!v)
        goto bail;
    while (v && cnt++ < 1000) {
        ocp = cp;
        nam = cp+1;
        isq = 0;
        if (*cp == '[') {
            cp = FindEndB(cp+1); /* handle [] in strings. */
            if (!cp) goto bail;
            len = cp-nam;
            cp++;
            if (len>=2 && ((nam[0] == '\"' && nam[len-1] == '\"') || (nam[0] == '\'' && nam[len-1] == '\''))) {
                nam += 1;
                len -= 2;
                isq = 1;
            }
        } else if (*cp == '.') {
            cp2 = Jsi_Strchr(nam,'[');
            cp = Jsi_Strchr(nam, '.');
            if (cp2 && (cp==0 || cp2<cp))
                cp = cp2;
            len = (cp ? (uint)(cp-nam) : Jsi_Strlen(nam));
        } else {
            goto bail;
        }
        Jsi_DSSetLength(&dStr, 0);
        Jsi_DSAppendLen(&dStr, nam, len);
        kstr = Jsi_DSValue(&dStr);
        if (*ocp == '[' && isq == 0 && isdigit(kstr[0]) && Jsi_ValueIsArray(interp, v)) {
            int nn;
            if (Jsi_GetInt(interp, kstr, &nn, 0) != JSI_OK)
                goto bail;
            v = Jsi_ValueArrayIndex(interp, v, nn);
            if (!v)
                goto bail;
        } else if (*ocp == '[' && isq == 0) {
            Jsi_Value *kv = Jsi_VarLookup(interp, kstr);
            if (!kv)
                goto bail;
            v = jsi_ValueSubscript(interp, v, kv, &nvPtr);
            goto keyon;
        } else {
            Jsi_ValueMakeStringKey(interp, &kPtr, kstr);
            v = jsi_ValueSubscript(interp, v, kPtr, &nvPtr);
keyon:
            if (!v)
                goto bail;
        }
        if (cp == 0 || *cp == 0) break;
    }
    //Jsi_ValueReset(interp, &ret);
    Jsi_DSFree(&dStr);
    if (v && v == nvPtr) {
        v = Jsi_ValueNew(interp);
        //Jsi_ValueMove(interp, v, &tv);
#ifdef JSI_MEM_DEBUG
        memcpy(v, &nv, sizeof(nv)-sizeof(nv.VD));
        v->VD.label3 = nv.VD.func;
        if (interp->memDebug>1)
            v->VD.label2 = Jsi_KeyAdd(interp, name);
#else
        *v = nv;
#endif
    }
    return v;
bail:
    Jsi_DSFree(&dStr);
    return NULL;
}