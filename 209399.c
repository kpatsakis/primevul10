static const char *jsi_evalprint(Jsi_Value *v)
{
    static char buf[JSI_MAX_NUMBER_STRING];
    if (!v)
        return "nil";
    if (v->vt == JSI_VT_NUMBER) {
        snprintf(buf, sizeof(buf), "NUM:%" JSI_NUMGFMT " ", v->d.num);
    } else if (v->vt == JSI_VT_BOOL) {
        snprintf(buf, sizeof(buf), "BOO:%d", v->d.val);
    } else if (v->vt == JSI_VT_STRING) {
        snprintf(buf, sizeof(buf), "STR:'%s'", v->d.s.str);
    } else if (v->vt == JSI_VT_VARIABLE) {
        snprintf(buf, sizeof(buf), "VAR:%p", v->d.lval);
    } else if (v->vt == JSI_VT_NULL) {
        snprintf(buf, sizeof(buf), "NULL");
    } else if (v->vt == JSI_VT_OBJECT) {
        snprintf(buf, sizeof(buf), "OBJ:%p", v->d.obj);
    } else if (v->vt == JSI_VT_UNDEF) {
        snprintf(buf, sizeof(buf), "UNDEFINED");
    }
    return buf;
}