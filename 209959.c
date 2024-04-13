static void jsi_DumpFunctions(Jsi_Interp *interp, const char *spnam) {
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_MapEntry *hPtr;
    Jsi_MapSearch search;
    Jsi_CmdSpecItem *csi = NULL;
    Jsi_CmdSpec *cs;
    Jsi_Value *lsf = interp->lastSubscriptFail;
    Jsi_Obj *lso = ((lsf && lsf->vt == JSI_VT_OBJECT)?lsf->d.obj:0);
    const char *varname = NULL;
    int m = 0;
    
    if (lso) {
        spnam = interp->lastSubscriptFailStr;
        if (!spnam) spnam = interp->lastPushStr;
        if (!spnam) spnam = "";
        if (lso->ot == JSI_OT_USEROBJ && lso->d.uobj->reg && lso->d.uobj->interp == interp) {
            cs = lso->d.uobj->reg->spec;
            if (cs)
                goto dumpspec;
        } else if (lso->ot == JSI_OT_FUNCTION) {
            cs = lso->d.fobj->func->cmdSpec;
            if (cs)
                goto dumpspec;
        } else if (lso->ot == JSI_OT_OBJECT) {
            jsiObjGetNames(interp, lso, &dStr, JSI_NAME_FUNCTIONS);
            Jsi_LogError("'%s', functions are: %s.",
                spnam, Jsi_DSValue(&dStr));
            Jsi_DSFree(&dStr);
            return;
        } else {
            const char *sustr = NULL;
            switch (lso->ot) {
                case JSI_OT_STRING: sustr = "String"; break;
                case JSI_OT_NUMBER: sustr = "Number"; break;
                case JSI_OT_BOOL: sustr = "Boolean"; break;
                default: break;
            }
            if (sustr) {
                hPtr = Jsi_MapEntryFind(interp->cmdSpecTbl, sustr);
                csi = (Jsi_CmdSpecItem*)Jsi_MapValueGet(hPtr);
                cs = csi->spec;
                if (!spnam[0])
                    spnam = sustr;
                goto dumpspec;
            }
        }
    }
    if (!spnam) spnam = "";
    if (!*spnam) {
        for (hPtr = Jsi_MapSearchFirst(interp->cmdSpecTbl, &search, 0);
            hPtr; hPtr = Jsi_MapSearchNext(&search)) {
            csi = (Jsi_CmdSpecItem*)Jsi_MapValueGet(hPtr);
            if (csi->name && csi->name[0])
                Jsi_DSAppend(&dStr, (m++?" ":""), csi->name, NULL);
        }
        Jsi_MapSearchDone(&search);
    }
    
    varname = spnam;
    if ((hPtr = Jsi_MapEntryFind(interp->cmdSpecTbl, spnam))) {
        csi = (Jsi_CmdSpecItem*)Jsi_MapValueGet(hPtr);
        while (csi) {
            int n;
            cs = csi->spec;
dumpspec:
            n = 0;
            while (cs->name) {
                if (n != 0 || !(cs->flags & JSI_CMD_IS_CONSTRUCTOR)) {
                    if (!*cs->name) continue;
                    Jsi_DSAppend(&dStr, (m?" ":""), cs->name, NULL);
                    n++; m++;
                }
                cs++;
            }
            csi = (csi?csi->next:NULL);
        }
        jsi_SortDString(interp, &dStr, " ");
        if (varname)
            spnam = varname;
        else if (interp->lastPushStr && !spnam[0])
            spnam = interp->lastPushStr;
        Jsi_LogError("'%s' sub-commands are: %s.",
            spnam, Jsi_DSValue(&dStr));
        Jsi_DSFree(&dStr);
    } else {
        Jsi_LogError("can not execute expression: '%s' not a function",
            varname ? varname : "");
    }
}