void Jsi_IterGetKeys(Jsi_Interp *interp, Jsi_Value *target, Jsi_IterObj *iterobj, int depth)
{
    if (!target) return;
    if (target->vt != JSI_VT_OBJECT) {
        if (interp->strict)
            Jsi_LogWarn("operand is not a object");
        return;
    }
    Jsi_Obj *to = target->d.obj;
    Jsi_CmdSpec *cs = NULL;
    if (to->ot == JSI_OT_USEROBJ) {
        Jsi_UserObj *uobj = to->d.uobj;
        cs = uobj->reg->spec;
    } else if (to->ot == JSI_OT_FUNCTION) {
        Jsi_FuncObj *fobj = to->d.fobj;
        if (fobj->func->type == FC_BUILDIN)
            cs = fobj->func->cmdSpec;
    }
    if (cs) {
        while (cs->name) {
            IterObjInsertKey(iterobj, cs->name);
            cs++;
        }
        return;
    }
    iterobj->depth = depth;
    Jsi_TreeWalk(target->d.obj->tree, IterGetKeysCallback, iterobj, 0);
    if (target->d.obj->__proto__ && target != target->d.obj->__proto__)
        Jsi_IterGetKeys(interp, target->d.obj->__proto__, iterobj, depth+1);
    iterobj->depth = depth;
}