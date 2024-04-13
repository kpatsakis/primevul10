static Jsi_RC StringConstructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    if (Jsi_FunctionIsConstructor(funcPtr)) {
        const char *nv = "";
        int len = -1;
        if (Jsi_ValueGetLength(interp, args) > 0) {
            Jsi_Value *v = Jsi_ValueArrayIndex(interp, args, 0);
            if (v) {
                
                nv = Jsi_ValueToString(interp, v, &len);
            }
        }
        if (_this->vt == JSI_VT_OBJECT && _this->d.obj->ot == JSI_OT_STRING) {
            if (_this->d.obj->d.s.str)
                if (!_this->d.obj->isstrkey)
                    Jsi_Free(_this->d.obj->d.s.str);
            _this->d.obj->isstrkey = 0;
            _this->d.obj->d.s.str = Jsi_StrdupLen(nv, len);
            _this->d.obj->d.s.len = len;
        } else
            jsi_ValueMakeBlobDup(interp, &_this, (uchar*)nv, len);
        Jsi_ValueDup2(interp, ret, _this);
        return JSI_OK;
    }
    if (Jsi_ValueGetLength(interp, args) > 0) {
        Jsi_Value *v = Jsi_ValueArrayIndex(interp, args, 0);
        if (v) {
            Jsi_ValueDup2(interp, ret, v);
            Jsi_ValueToString(interp, *ret, NULL);
            return JSI_OK;
        }
    }
    Jsi_ValueMakeStringDup(interp, ret, "");
    return JSI_OK;
}