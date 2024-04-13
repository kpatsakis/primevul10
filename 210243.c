static Jsi_RC jsi_ValueToBitset(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    // TODO: change this to not use byte instead of int...
    int i, argc, n;
    char *s =(((char*)record) + spec->offset);
    char **argv;
    const char *cp, **list = (const char**)spec->data;
    uint64_t m = 0, im = 0;
    int fflags = ((flags|spec->flags)&JSI_OPT_CUST_NOCASE?JSI_CMP_NOCASE:0);
    if (!list) 
        return Jsi_LogError("custom enum spec did not set data: %s", spec->name);
    switch (spec->size) {
        case 1: im = *(uint8_t*)s; break;
        case 2: im = *(uint16_t*)s; break;
        case 4: im = *(uint32_t*)s; break;
        case 8: im = *(uint64_t*)s; break;
        default: 
            return Jsi_LogError("bitset size must be 1, 2, 4, or 8: %s", spec->name);
    }

#ifndef JSI_LITE_ONLY
    if (!inStr && Jsi_ValueIsString(interp, inValue))
        inStr = Jsi_ValueString(interp, inValue, NULL);
#endif
    if (inStr) {
        if (*inStr == '+') {
            inStr++;
            m = im;
        }
        if (*inStr) {
            Jsi_DString sStr;
            Jsi_DSInit(&sStr);
            Jsi_SplitStr(inStr, &argc, &argv, ",", &sStr);
            
            for (i=0; i<argc; i++) {
                int isnot = 0;
                cp = argv[i];
                if (*cp == '!') { isnot = 1; cp++; }
                if (JSI_OK != Jsi_GetIndex(interp, cp, list, "enum", fflags, &n))
                    return JSI_ERROR;
                if (n >= (int)(spec->size*8)) 
                    return Jsi_LogError("list larger than field size: %s", spec->name);
                if (isnot)
                    m &= ~(1<<n);
                else
                    m |= (1<<n);
            }
            Jsi_DSFree(&sStr);
        }
    } else {
#ifndef JSI_LITE_ONLY
        if (!inValue) {
            *s = 0;
            return JSI_OK;
        }
        if (Jsi_ValueIsObjType(interp, inValue, JSI_OT_OBJECT) && !Jsi_ValueIsArray(interp, inValue)) {
            Jsi_TreeEntry *tPtr;
            Jsi_TreeSearch search;
            Jsi_Tree *tp = Jsi_TreeFromValue(interp, inValue);
            
            m = im;
            for (tPtr = (tp?Jsi_TreeSearchFirst(tp, &search, 0, NULL):NULL);
                tPtr != NULL; tPtr = Jsi_TreeSearchNext(&search)) {
                cp =(char*) Jsi_TreeKeyGet(tPtr);
                Jsi_Value *optval = (Jsi_Value*)Jsi_TreeValueGet(tPtr);
                
                if (JSI_OK != Jsi_GetIndex(interp, cp, list, "bitset", fflags, &n)) {
                    Jsi_TreeSearchDone(&search);
                    return JSI_ERROR;
                }
                if (!Jsi_ValueIsBoolean(interp, optval)) {
                    Jsi_TreeSearchDone(&search);
                    return Jsi_LogError("object member is not a bool: %s", cp);
                }
                bool vb;
                Jsi_ValueGetBoolean(interp, optval, &vb);
                if (!vb)
                    m &= ~(1<<n);
                else
                    m |= (1<<n);
            }
            if (tp)
                Jsi_TreeSearchDone(&search);
            *s = m;
            return JSI_OK;
        }
        
        if (!Jsi_ValueIsArray(interp, inValue)) 
            return Jsi_LogError("expected array or object");
        argc = Jsi_ValueGetLength(interp, inValue);
        for (i=0; i<argc; i++) {
            int isnot = 0;
            Jsi_Value *v = Jsi_ValueArrayIndex(interp, inValue, i);
            const char *cp = (v?Jsi_ValueString(interp, v, NULL):"");
            if (!cp) 
                return Jsi_LogError("expected string");
            if (i == 0) {
                if (*cp == '+' && !cp[1]) {
                    m = im;
                    continue;
                }
            }
            if (*cp == '!') { isnot = 1; cp++; }
            if (JSI_OK != Jsi_GetIndex(interp, cp, list, "bitset", fflags, &n))
                return JSI_ERROR;
            if (isnot)
                m &= ~(1<<n);
            else
                m |= (1<<n);
        }
        *s = m;
#endif
    }
    switch (spec->size) {
        case 1: *(uint8_t*)s = (uint8_t)m; break;
        case 2: *(uint16_t*)s = (uint16_t)m; break;
        case 4: *(uint32_t*)s = (uint32_t)m; break;
        case 8: *(uint64_t*)s = (uint64_t)m; break;
    }
    return JSI_OK;
}