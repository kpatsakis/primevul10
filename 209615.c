static Jsi_RC Jsi_DoneMySql(Jsi_Interp *interp)
{
    if (Jsi_UserObjUnregister(interp, &mysqlobject) != JSI_OK)
        return JSI_ERROR;
    Jsi_PkgProvide(interp, "MySql", -1, NULL);
    return JSI_OK;
}