static void jsi_ValueToPrimitiveRes(Jsi_Interp *interp, Jsi_Value *v, Jsi_Value *rPtr)
{
    if (v->vt != JSI_VT_OBJECT) {
#ifdef JSI_MEM_DEBUG
    memcpy(rPtr, v, sizeof(*v)-sizeof(v->VD));
#else
    *rPtr = *v; //TODO: usde only by ValueCompare, so refCnt doesn't matter?
#endif
        return;
    }
    Jsi_Obj *obj = v->d.obj;
    switch(obj->ot) {
        case JSI_OT_BOOL:
            Jsi_ValueMakeBool(interp, &rPtr, obj->d.val);
            break;
        case JSI_OT_NUMBER:
            Jsi_ValueMakeNumber(interp, &rPtr, obj->d.num);
            break;
        case JSI_OT_STRING:
            rPtr->vt = JSI_VT_STRING;
            rPtr->d.s = obj->d.s;
            rPtr->f.bits.isstrkey = 1;
            break;
        default:
            break;
    }
}