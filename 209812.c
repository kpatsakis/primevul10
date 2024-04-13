bool Jsi_ValueIsStringKey(Jsi_Interp* interp, Jsi_Value *key)
{
    if (key->vt == JSI_VT_STRING && key->f.bits.isstrkey)
        return 1;
    if (key->vt == JSI_VT_OBJECT && key->d.obj->ot == JSI_OT_STRING && key->d.obj->isstrkey)
        return 1;
    return 0;
}