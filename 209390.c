Jsi_RC Jsi_Mount( Jsi_Interp *interp, Jsi_Value *archive, Jsi_Value *mount, Jsi_Value **ret)
{
#if JSI__ZVFS==1
    return Zvfs_Mount(interp, archive, mount, ret);
#else
    return JSI_ERROR;
#endif
}