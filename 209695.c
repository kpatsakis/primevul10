static Jsi_RC jsi_evalSubscript(Jsi_Interp *interp, Jsi_Value *src, Jsi_Value *idx, jsi_OpCode *ip,  jsi_OpCode *end,
    Jsi_Value *currentScope)
{
    Jsi_RC rc = JSI_OK;
    jsiVarDeref(interp,2);
    int isnull;
    if ((isnull=Jsi_ValueIsNull(interp, src)) || Jsi_ValueIsUndef(interp, src)) {
        Jsi_LogError("invalid subscript of %s", (isnull?"null":"undefined"));
        jsiPop(interp, 1);
        return JSI_ERROR;
    }
    Jsi_String *str = jsi_ValueString(src);
    if (str && Jsi_ValueIsNumber(interp, idx)) {
        int bLen, cLen;
        char bbuf[10], *cp = Jsi_ValueString(interp, src, &bLen);
        int n = (int)idx->d.num;
        cLen = bLen;
#if JSI__UTF8
        if (str->flags&JSI_IS_UTF || !(str->flags&JSI_UTF_CHECKED)) {
            cLen = Jsi_NumUtfChars(cp, bLen);
            str->flags |= JSI_UTF_CHECKED;
            if (cLen != bLen)
                str->flags |= JSI_IS_UTF;
        }
#endif
        if (n<0 || n>=cLen) {
            Jsi_ValueMakeUndef(interp, &src);
        } else {
            if (cLen != bLen)
                Jsi_UtfGetIndex(cp, n, bbuf);
            else {
                bbuf[0] = cp[n];
                bbuf[1] = 0;
            }
            Jsi_ValueMakeStringDup(interp, &src, bbuf);
        }
        jsiPop(interp, 1);
        return rc;
    }
    Jsi_ValueToObject(interp, src);
    if (interp->hasCallee && (src->d.obj == currentScope->d.obj || (interp->framePtr->arguments && src->d.obj == interp->framePtr->arguments->d.obj))) {
        if (idx->vt == JSI_VT_STRING && Jsi_Strcmp(idx->d.s.str, "callee") == 0) {
            jsiClearStack(interp,1);
            Jsi_ValueMakeStringKey(interp, &idx, "\1callee\1");
        }
    }
    int bsc = Jsi_ValueIsObjType(interp, src, JSI_OT_NUMBER); // Previous bad subscript.
    if (bsc == 0 && interp->lastSubscriptFail && interp->lastSubscriptFail->vt != JSI_VT_UNDEF)
        Jsi_ValueReset(interp, &interp->lastSubscriptFail);

    if (src->vt != JSI_VT_UNDEF) {
        int right_val = (uintptr_t)ip->data;
        Jsi_Value res = VALINIT, 
            *resPtr = &res,
            *vp = jsi_ValueSubscript(interp, src, idx, &resPtr);
        if (!vp && bsc == 0) {
            /* eg. so we can list available commands for  "db.xx()" */
            if (idx->vt == JSI_VT_STRING)
                interp->lastSubscriptFailStr = idx->d.s.str;
            Jsi_ValueDup2(interp, &interp->lastSubscriptFail, src);
        }
        if (vp)
            Jsi_IncrRefCount(interp, vp);
        jsiClearStack(interp,2);
        if (!vp)
            Jsi_ValueMakeUndef(interp, &src);
        else {
            //printf("IDX(%p): %s\n", idx, Jsi_ValueString(interp, idx, NULL));
            if (right_val || vp->f.bits.readonly) {
                if (vp == resPtr && (res.vt == JSI_VT_OBJECT || res.vt == JSI_VT_STRING))  // TODO:*** Undo using ValueCopy. ***
                    Jsi_ValueMove(interp, src, vp);
                else
                    Jsi_ValueCopy(interp, src, vp);
            } else {
                assert(vp != resPtr);
                res.vt = JSI_VT_VARIABLE;
                res.d.lval = vp;
                Jsi_ValueCopy(interp, src, resPtr);
            }
            Jsi_DecrRefCount(interp, vp);
        }
    }
    jsiPop(interp, 1);
    return rc;
}