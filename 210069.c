static const char **jsi_OptGetEnumList(Jsi_OptionSpec* spec) {
    const char **list = (const char**)spec->data;
    Jsi_OptionSpec* es=NULL;
    int fflags = (spec->flags);
    if (spec->id != JSI_OPTION_CUSTOM || spec->custom != Jsi_Opt_SwitchEnum)
        return list;
    if (list && (fflags & JSI_OPT_ENUM_SPEC)) {
        es = (typeof(es))list;
        while (es->id != JSI_OPTION_END)
            es++;
        list = es->init.STRKEY;
    }
    return list;
}