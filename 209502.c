void jsi_ValueToPrimitive(Jsi_Interp *interp, Jsi_Value **vPtr)
{
    Jsi_Value *v = *vPtr;
    if (v->vt != JSI_VT_OBJECT)
        return;
    DECL_VALINIT(res);
    Jsi_Value *rPtr = &res;
    Jsi_Obj *obj = v->d.obj;
    //rPtr = v;
    switch(obj->ot) {
        case JSI_OT_BOOL:
            Jsi_ValueMakeBool(interp,&rPtr, obj->d.val);
            break;
        case JSI_OT_NUMBER:
            Jsi_ValueMakeNumber(interp,&rPtr, obj->d.num);
            break;
        case JSI_OT_STRING:
            if (obj->isstrkey) {
                res.d.s = obj->d.s;
                res.f.bits.isstrkey = 1;
                obj->d.s.str = NULL;
            } else {
                if (obj->refcnt==1) {
                    Jsi_ValueMakeString(interp, &rPtr, obj->d.s.str);
                    res.d.s = obj->d.s;
                    obj->d.s.str = NULL;
                } else if (obj->d.s.len >= 0) 
                {
                    Assert(obj->refcnt>=1);
                    obj->refcnt--;
                    int bytes = obj->d.s.len;
                    jsi_ValueMakeBlobDup(interp, &rPtr, (uchar*)obj->d.s.str, bytes);
                } else
                    Jsi_ValueMakeStringDup(interp, &rPtr, obj->d.s.str);
            }
            break;
        case JSI_OT_FUNCTION: {
            Jsi_DString dStr;
            Jsi_DSInit(&dStr);
            Jsi_FuncObjToString(interp, obj->d.fobj->func, &dStr, 3);
            Jsi_ValueFromDS(interp, &dStr, &rPtr);
            break;
        }
        case JSI_OT_USEROBJ: {
            Jsi_DString dStr;
            Jsi_DSInit(&dStr);
            jsi_UserObjToName(interp, obj->d.uobj, &dStr);
            Jsi_ValueFromDS(interp, &dStr, &rPtr);
            break;
        }
        default:
            Jsi_ValueMakeStringKey(interp,&rPtr, "[object Object]");
            break;
    }
    Jsi_ValueReset(interp, vPtr);
    res.refCnt = v->refCnt;
#ifdef JSI_MEM_DEBUG
    memcpy(v, &res, sizeof(res)-sizeof(res.VD));
#else
    *v = res;
#endif
}