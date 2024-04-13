int jsi_BuiltinCmd(Jsi_Interp *interp, const char *name)
{
    Jsi_Value *val = Jsi_NameLookup(interp, name);
    if (!name)
        return 0;
    if (!Jsi_ValueIsFunction(interp, val))
        return 0;
    Jsi_Func *f = val->d.obj->d.fobj->func;
    return (f->type == FC_BUILDIN);
}