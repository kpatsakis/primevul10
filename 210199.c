const char *jsi_OptionTypeStr(Jsi_OptionId typ, bool cName)
{
    const Jsi_OptionTypedef* ti = Jsi_OptionTypeInfo(typ);
    if (ti)
        return (cName?ti->cName:ti->idName);
    return NULL;
}