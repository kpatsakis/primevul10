int Jsi_ValueStrlen(Jsi_Value* v) {
    //if (v->vt == JSI_VT_OBJECT && v->d.obj->ot == JSI_OT_STRING && v->d.obj->isBlob)
    //    return v->d.obj->d.s.len;
    Jsi_String *s = jsi_ValueString(v);
    if (s == 0 || s->str == 0)
        return 0;
#if JSI__UTF8
    return (int)Jsi_NumUtfChars(s->str, s->len);
#else
    if (s->len>=0) return s->len;
    return (int)Jsi_NumUtfChars(s->str, s->len);
#endif
}