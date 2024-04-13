static Jsi_RC FilesysConstructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Value *that = _this;
    if (!Jsi_FunctionIsConstructor(funcPtr)) {
        Jsi_Obj *o = Jsi_ObjNew(interp);
        Jsi_PrototypeObjSet(interp, "File", o);
        Jsi_ValueMakeObject(interp, ret, o);
        that = *ret;
    }

    FileObj *fobj = (FileObj *)Jsi_Calloc(1,sizeof(*fobj));
    SIGINIT(fobj, FILEOBJ);
    if (try_open_file(interp, fobj, args) != JSI_OK) { /* Error out on open fail */
        Jsi_Free(fobj);
        return JSI_ERROR;
    }
    Jsi_Obj *nobj = (Jsi_Obj*)Jsi_ValueGetObj(interp, that);
    fobj->objId = Jsi_UserObjNew(interp, &fileobject, nobj, fobj);
    if (fobj->objId<0) {
        Jsi_Free(fobj); // TODO: finish cleanup
        return JSI_ERROR;
    }
    fobj->fobj = nobj;
    return JSI_OK;
}