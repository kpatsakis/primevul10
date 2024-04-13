static Jsi_RC jsi_logic_less(Jsi_Interp* interp, int i1, int i2) {
    Jsi_Value *v, *v1 = _jsi_STACK[interp->framePtr->Sp-i1], *v2 = _jsi_STACK[interp->framePtr->Sp-i2], *res = _jsi_TOQ;
    int val = 0, l1 = 0, l2 = 0; 
    bool strict = interp->strict;
    Jsi_RC rc = JSI_OK;
    rc = _jsi_StrictUChk2(v1, v2);
    if (rc != JSI_OK)
        return JSI_ERROR;
    char *s1 = Jsi_ValueString(interp, v1, &l1);
    char *s2 = Jsi_ValueString(interp, v2, &l2);
    Jsi_Number n1, n2;

    if (s1 || s2) {
        char *str;
        if (!(s1 && s2)) {
            v = (s1 ? v2 : v1);
            jsi_ValueToPrimitive(interp, &v);
            Jsi_ValueToString(interp, v, NULL);
            str = Jsi_ValueString(interp, v, (s1?&l2:&l1));
            if (s1) s2 = str; else s1 = str;
        }
        Assert(l1>=0 && l1<=JSI_MAX_ALLOC_BUF);
        Assert(l2>=0 && l2<=JSI_MAX_ALLOC_BUF);
        //int mlen = (l1>l2?l1:l2);
        val = Jsi_Strcmp(s1, s2);
  
        if (val > 0) val = 0;
        else if (val < 0) val = 1;
        else val = (l1 < l2);
        jsiClearStack(interp,2);
        Jsi_ValueMakeBool(interp, &res, val);
    } else {
        Jsi_ValueToNumber(interp, v1);
        Jsi_ValueToNumber(interp, v2);
        rc = _jsi_StrictChk2(v1,v2);
        if (rc != JSI_OK)
            return JSI_ERROR;
        n1 = v1->d.num; n2 = v2->d.num;
        if (Jsi_NumberIsNaN(n1) || Jsi_NumberIsNaN(n2)) {
            jsiClearStack(interp,2);
            Jsi_ValueMakeUndef(interp, &res);
        } else {
            val = (n1 < n2);
            jsiClearStack(interp,2);
            Jsi_ValueMakeBool(interp, &res, val);
        }
    }
    return JSI_OK;
}