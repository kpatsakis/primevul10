Jsi_Channel Jsi_FSNameToChannel(Jsi_Interp *interp, const char *name)
{
    if (Jsi_Strlen(name)==1) {
        switch (name[0]) {
            case '0' : return jsiIntData.stdChans;
            case '1' : return jsiIntData.stdChans+1;
            case '2' : return jsiIntData.stdChans+2;
        }
    }
    if (!Jsi_Strcmp(name, "stdin")) return jsiIntData.stdChans;
    if (!Jsi_Strcmp(name, "stdout")) return jsiIntData.stdChans+1;
    if (!Jsi_Strcmp(name, "stderr")) return jsiIntData.stdChans+2;
    Jsi_Obj *obj = jsi_UserObjFromName(interp, name);
    if (!obj)
        return NULL;
    Jsi_UserObj *uobj = obj->d.uobj;
    if (uobj->reg != &fileobject)
        return NULL;
    FileObj *fobj = (FileObj *)uobj->data;
    return fobj->chan;
}