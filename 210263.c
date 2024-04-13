static Jsi_RC jsi_AliasCreateCmd(Jsi_Interp* interp, const char* key, AliasCmd* ac) {
    if (Jsi_InterpGone(interp))
        return JSI_ERROR;
    key = Jsi_KeyAdd(interp, key);
    Jsi_Value *cmd = jsi_CommandCreate(interp, key, jsi_AliasInvoke, NULL, 0, 0);
    if (!cmd)
        return Jsi_LogBug("command create failure");
    ac->cmdVal = cmd;
    Jsi_Func *fobj = cmd->d.obj->d.fobj->func;
    fobj->cmdSpec->reserved[2] = ac;
    cmd->d.obj->isNoOp = (ac->func->d.obj->d.fobj->func->callback == jsi_NoOpCmd);
    return JSI_OK;
}