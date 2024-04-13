Jsi_Value *jsi_MakeFuncValueSpec(Jsi_Interp *interp, Jsi_CmdSpec *cmdSpec, void *privData)
{
    Jsi_Obj *o = Jsi_ObjNew(interp);
    Jsi_Func *f = jsi_FuncNew(interp);
    o->ot = JSI_OT_FUNCTION;
    f->type = FC_BUILDIN;
    f->cmdSpec = cmdSpec;
    f->callback = cmdSpec->proc;
    f->privData = privData;
    f->f.flags = (cmdSpec->flags & JSI_CMD_MASK);
    f->script = interp->curFile;
    o->d.fobj = jsi_FuncObjNew(interp, f);
    return Jsi_ValueMakeObject(interp, NULL, o);
}