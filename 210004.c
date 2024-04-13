void jsi_DebugDumpValues(Jsi_Interp *interp)
{
    if (jsiIntData.mainInterp != interp) return;
    int vdLev = interp->memDebug;
    int have = (interp->dbPtr->valueDebugTbl->numEntries || interp->dbPtr->objDebugTbl->numEntries);
    if ((have && vdLev>0) || vdLev>=3) {
        // First traverse all Object trees/arrays and mark all values contained therein.
        Jsi_HashSearch search;
        Jsi_HashEntry *hPtr;
        for (hPtr = Jsi_HashSearchFirst(interp->dbPtr->objDebugTbl, &search);
            hPtr != NULL; hPtr = Jsi_HashSearchNext(&search)) {
            Jsi_Obj *vp = (Jsi_Obj *)Jsi_HashKeyGet(hPtr);
            if (vp!=NULL && vp->sig == JSI_SIG_OBJ) {
                jsiFlagDebugValues(interp, vp);
            }
        }
        if (interp->dbPtr->valueDebugTbl->numEntries != interp->dbPtr->valueCnt)
            fprintf(stderr, "\n\nValues table/alloc mismatch: table=%d, alloc=%d\n",
                interp->dbPtr->valueDebugTbl->numEntries, interp->dbPtr->valueCnt);
        // Dump unfreed values and objs.
        int refSum=0, refsum=0;
        int bcnt[4] = {};
        if (vdLev>1 && interp->dbPtr->valueDebugTbl->numEntries)
            fprintf(stderr, "\n\nUNFREED VALUES \"[*ptr,#refCnt,type,idx:label,label2]: @file:line in func() ...\"\n");
        for (hPtr = Jsi_HashSearchFirst(interp->dbPtr->valueDebugTbl, &search);
            hPtr != NULL; hPtr = Jsi_HashSearchNext(&search)) {
            Jsi_Value *vp = (Jsi_Value *)Jsi_HashKeyGet(hPtr);
            if (vp==NULL || vp->sig != JSI_SIG_VALUE) {
                bcnt[0]++;
                if (vdLev>1)
                    fprintf(stderr, "BAD VALUE: %p\n", vp);
            } else {
                bcnt[1]++;
                refSum += vp->refCnt;
                if (vdLev>1) {
                    char ebuf[JSI_BUFSIZ], ebuf2[JSI_MAX_NUMBER_STRING];
                    ebuf[0] = 0;
                    if (vp->vt==JSI_VT_OBJECT)
                        snprintf(ebuf, sizeof(ebuf), " {obj=%p, otype=%s}", vp->d.obj, Jsi_ObjTypeStr(interp, vp->d.obj));
                    else if (vp->vt==JSI_VT_NUMBER)
                        snprintf(ebuf, sizeof(ebuf), " {num=%s}", Jsi_NumberToString(interp, vp->d.num, ebuf2, sizeof(ebuf2)));
                    else if (vp->vt==JSI_VT_BOOL)
                        snprintf(ebuf, sizeof(ebuf), " {bool=%s}", vp->d.val?"true":"false");
                    else if (vp->vt==JSI_VT_STRING) {
                        const char *sbuf = ((vp->d.s.str && Jsi_Strlen(vp->d.s.str)>40)?"...":"");
                        snprintf(ebuf, sizeof(ebuf), " {string=\"%.40s%s\"}", (vp->d.s.str?vp->d.s.str:""), sbuf);
                    }
                    const char *pfx = "";
                    if (!(vp->VD.flags&MDB_INOBJ))
                        pfx = "!"; // Value is not contained in an object.
                    fprintf(stderr, "[%s*%p,#%d,%s,%d:%s%s%s]:%s @%s:%d in %s()%s\n", pfx,
                        vp, vp->refCnt, Jsi_ValueTypeStr(interp, vp), vp->VD.Idx,
                        (vp->VD.label?vp->VD.label:""), (vp->VD.label2?":":""),
                        (vp->VD.label2?vp->VD.label2:""), vp->VD.interp==jsiIntData.mainInterp?"":"!",
                        vp->VD.fname, vp->VD.line, vp->VD.func, ebuf);
                }
            }
        }
        if (interp->dbPtr->objDebugTbl->numEntries != interp->dbPtr->objCnt)
            fprintf(stderr, "\n\nObject table/alloc mismatch: table=%d, alloc=%d\n",
                interp->dbPtr->objDebugTbl->numEntries, interp->dbPtr->objCnt);
        if (vdLev>1 && interp->dbPtr->objDebugTbl->numEntries)
            fprintf(stderr, "\n\nUNFREED OBJECTS \"[*ptr,#refCnt,type,idx:label,label2]: @file:line in func() ...\"\n");
        for (hPtr = Jsi_HashSearchFirst(interp->dbPtr->objDebugTbl, &search);
            hPtr != NULL; hPtr = Jsi_HashSearchNext(&search)) {
            Jsi_Obj *vp = (Jsi_Obj *)Jsi_HashKeyGet(hPtr);
            if (vp==NULL || vp->sig != JSI_SIG_OBJ) {
                bcnt[2]++;
                fprintf(stderr, "BAD OBJ: %p\n", vp);
            } else {
                bcnt[3]++;
                refsum += vp->refcnt;
                if (vdLev>1) {
                    char ebuf[JSI_BUFSIZ], ebuf2[JSI_MAX_NUMBER_STRING];
                    ebuf[0] = 0;
                    if (vp->ot==JSI_OT_OBJECT) {
                        if (vp->isarrlist)
                            snprintf(ebuf, sizeof(ebuf), "tree#%d, array#%d", (vp->tree?vp->tree->numEntries:0), vp->arrCnt);
                        else
                            snprintf(ebuf, sizeof(ebuf), "tree#%d", (vp->tree?vp->tree->numEntries:0));
                    } else if (vp->ot==JSI_OT_NUMBER)
                        snprintf(ebuf, sizeof(ebuf), "num=%s", Jsi_NumberToString(interp, vp->d.num, ebuf2, sizeof(ebuf2)));
                    else if (vp->ot==JSI_OT_BOOL)
                        snprintf(ebuf, sizeof(ebuf), "bool=%s", vp->d.val?"true":"false");
                    else if (vp->ot==JSI_OT_STRING) {
                        const char *sbuf = ((vp->d.s.str && Jsi_Strlen(vp->d.s.str)>40)?"...":"");
                        snprintf(ebuf, sizeof(ebuf), "string=\"%.40s%s\"", (vp->d.s.str?vp->d.s.str:""), sbuf);
                    }
                    fprintf(stderr, "[*%p,#%d,%s,%d:%s%s%s]:%s @%s:%d in %s() {%s}\n",
                        vp, vp->refcnt, Jsi_ObjTypeStr(interp, vp), vp->VD.Idx, vp->VD.label?vp->VD.label:"",
                        vp->VD.label2?":":"",vp->VD.label2?vp->VD.label2:"", vp->VD.interp==jsiIntData.mainInterp?"":"!",
                        vp->VD.fname, vp->VD.line,
                        vp->VD.func, ebuf);
                }
            }
        }
        fprintf(stderr, "\nVALUES: bad=%d,unfreed=%d,allocs=%d,refsum=%d  | OBJECTS: bad=%d,unfreed=%d,allocs=%d,refsum=%d  interp=%p\n",
            bcnt[0], bcnt[1], interp->dbPtr->valueAllocCnt, refSum, bcnt[2], bcnt[3], interp->dbPtr->objAllocCnt, refsum, interp);

        if (interp->codesTbl)
            for (hPtr = Jsi_HashSearchFirst(interp->codesTbl, &search);
                hPtr != NULL; hPtr = Jsi_HashSearchNext(&search)) {
                Jsi_OpCodes *vp = (Jsi_OpCodes *)Jsi_HashKeyGet(hPtr);
                fprintf(stderr, "unfreed opcodes: %d\n", vp->id);
            }
    }
    Jsi_HashDelete(interp->dbPtr->valueDebugTbl);
    Jsi_HashDelete(interp->dbPtr->objDebugTbl);
    Jsi_HashDelete(interp->codesTbl);
    bool isMainInt = (interp == jsiIntData.mainInterp);
    if (isMainInt && vdLev>3)
        _exit(1); // Avoid sanitize output.
}