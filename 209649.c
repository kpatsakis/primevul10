bool Jsi_ValueIsTrue(Jsi_Interp *interp, Jsi_Value *v)
{
    switch(v->vt) {
        case JSI_VT_UNDEF:
        case JSI_VT_NULL:   return 0;
        case JSI_VT_BOOL:   return v->d.val ? 1:0;
        case JSI_VT_NUMBER: 
            if (v->d.num == 0.0 || Jsi_NumberIsNaN(v->d.num)) return 0;
            return 1;
        case JSI_VT_STRING: return (Jsi_ValueStrlen(v)!=0);
        case JSI_VT_OBJECT: {
            Jsi_Obj *o = v->d.obj;
            if (o->ot == JSI_OT_STRING)
                return (Jsi_ValueStrlen(v)!=0);
            if (o->ot == JSI_OT_NUMBER)
                return (o->d.num != 0);
            if (o->ot == JSI_OT_BOOL)
                return (o->d.val != 0);
            if (o->ot == JSI_OT_USEROBJ && o->d.uobj->interp == interp) {
                return jsi_UserObjIsTrue(interp, o->d.uobj);
            }
            return 1;
        }
        default: Jsi_LogBug("TOP is type incorrect: %d", v->vt);
    }
    return 0;
}