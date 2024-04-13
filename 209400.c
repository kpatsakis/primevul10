static void jsi_wsDumpHeaders(jsi_wsCmdObj *cmdPtr, jsi_wsPss *pss, const char *name, Jsi_Value **ret)
{
    Jsi_Interp *interp = cmdPtr->interp;
    Jsi_Obj *nobj;
    Jsi_Value *nv;
    if (pss->hdrNum<=0)
        return;
    if (!name) {
        nobj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
        Jsi_ValueMakeObject(interp, ret, nobj);
        int nsiz = Jsi_ObjArraySizer(interp, nobj, pss->hdrNum);
        if (nsiz < pss->hdrNum) {
            printf("header failed, %d != %d", nsiz, pss->hdrNum);
            return;
        }
    }
    Jsi_DString dStr = {}, vStr = {};
    int i;
    const char *nam, *val, *cp = Jsi_DSValue(&pss->dHdrs);
    for (i=0; i<pss->hdrNum; i+=2) {
        int sz = pss->hdrSz[i], sz2 = pss->hdrSz[i+1];
        Jsi_DSSetLength(&dStr, 0);
        Jsi_DSSetLength(&vStr, 0);
        nam = Jsi_DSAppendLen(&dStr, cp, sz);
        cp += 1 + sz;
        val = Jsi_DSAppendLen(&vStr, cp, sz2);
        if (name) {
            if (!Jsi_Strcmp(nam, name)) {
                Jsi_ValueMakeStringDup(interp, ret, val);
                break;
            }
        } else {
            nv = Jsi_ValueNewStringDup(interp, val);
            Jsi_ObjArraySet(interp, nobj, Jsi_ValueNewStringDup(interp, nam), i);
            Jsi_ObjArraySet(interp, nobj, nv, i+1);
        }
        cp += (1 + sz2);
    }
    Jsi_DSFree(&dStr);
    Jsi_DSFree(&vStr);
}