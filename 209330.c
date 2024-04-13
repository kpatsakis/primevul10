int Jsi_ValueCmp(Jsi_Interp *interp, Jsi_Value *v1, Jsi_Value* v2, int flags)
{
    DECL_VALINIT(res1);
    DECL_VALINIT(res2);
    int r = 1;
    int nocase = (flags&JSI_SORT_NOCASE), dict = ((flags & JSI_SORT_DICT));
    if (v1 == v2)
        return 1;
    if (v1->vt != v2->vt) {
        jsi_ValueToPrimitiveRes(interp, v1, &res1);
        jsi_ValueToPrimitiveRes(interp, v2, &res2);
        v1 = &res1;
        v2 = &res2;
    }
    if (v1->vt != v2->vt) {
        if ((flags&JSI_CMP_EXACT))
            return 1;
        if ((v1->vt == JSI_VT_UNDEF || v1->vt == JSI_VT_NULL) && 
            (v2->vt == JSI_VT_UNDEF || v2->vt == JSI_VT_NULL)) {
            r = 0;
        } else {
            Jsi_Number n1, n2;
            n1 = Jsi_ValueToNumberInt(interp, v1, 0);
            n2 = Jsi_ValueToNumberInt(interp, v2, 0);
            r = (n2 - n1);
        }
    } else {
        switch (v1->vt) {
            case JSI_VT_NUMBER:
                if (v2->d.num == v1->d.num) return 0;
                r = (v2->d.num < v1->d.num ? -1 : 1);
                break;
            case JSI_VT_BOOL:
                r = (v2->d.val - v1->d.val);
                break;
            case JSI_VT_STRING:
                r = (Jsi_StrcmpDict(v2->d.s.str, v1->d.s.str, nocase, dict));
                break;
            case JSI_VT_OBJECT:
                /* TODO: refer to objects joined to each other */
                if (v2->vt != JSI_VT_OBJECT)
                    r = 1;
                else if (v1->d.obj->ot == JSI_OT_STRING && v2->d.obj->ot == JSI_OT_STRING)
                    r = (Jsi_StrcmpDict(v2->d.obj->d.s.str, v1->d.obj->d.s.str, nocase, dict));
                else
                    r = (v2->d.obj - v1->d.obj);
                break;
            case JSI_VT_UNDEF:
            case JSI_VT_NULL:
                r = 0;
                break;
            default:
                Jsi_LogBug("Unexpected value type");
        }
    }
    return r;
}