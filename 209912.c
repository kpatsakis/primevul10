Jsi_Value *jsi_MakeFuncValue(Jsi_Interp *interp, Jsi_CmdProc *callback, const char *name, Jsi_Value** toVal, Jsi_CmdSpec *cspec)
{
    Jsi_Obj *o = Jsi_ObjNew(interp);
    Jsi_Func *f = jsi_FuncNew(interp);
    Jsi_ObjIncrRefCount(interp, o);
    o->ot = JSI_OT_FUNCTION;
    f->type = FC_BUILDIN;
    f->callback = callback;
    f->privData = NULL;
    o->d.fobj = jsi_FuncObjNew(interp, f);
    f->cmdSpec = cspec;
    if (!cspec) {
        f->cmdSpec = (Jsi_CmdSpec*)Jsi_Calloc(2,sizeof(Jsi_CmdSpec));
        f->cmdSpec->reserved[3] = (void*)0x1;
        f->cmdSpec->maxArgs = -1;
        if (name)
            f->cmdSpec->name = (char*)Jsi_KeyAdd(interp, name);
    }
    f->script = interp->curFile;
    f->callback = callback;
    return Jsi_ValueMakeObject(interp, toVal, o);
}