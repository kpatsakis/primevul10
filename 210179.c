const char *jsi_DbOptionTypeStr(Jsi_OptionId typ, bool cname)
{
    const Jsi_OptionTypedef* ti = Jsi_OptionTypeInfo(typ);
    if (ti)
        return (cname?ti->cName:ti->idName);
    return NULL;
}