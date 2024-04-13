static void jsi_DumpCustomSpec(Jsi_Interp *interp, Jsi_Obj *nobj, Jsi_OptionSpec* spec)
{
    Jsi_OptionCustom* cspec = Jsi_OptionCustomBuiltin(spec->custom);
    void *data = spec->data;
    if (cspec->help) {
        if (Jsi_Strchr(cspec->help, '\n'))
            Jsi_LogError("%s .help contains newline: %s", cspec->name, cspec->help);
        Jsi_ObjInsert(interp, nobj, "help", Jsi_ValueNewStringKey(interp, cspec->help),0);
    }
    if (cspec->info)
        Jsi_ObjInsert(interp, nobj, "info", Jsi_ValueNewStringKey(interp, cspec->info),0);
    Jsi_ObjInsert(interp, nobj, "name", Jsi_ValueNewStringKey(interp, cspec->name),0);

    if (data && (spec->custom == Jsi_Opt_SwitchEnum || spec->custom == Jsi_Opt_SwitchBitset)) {
        Jsi_Obj *sobj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
        Jsi_Value *svalue = Jsi_ValueMakeObject(interp, NULL, sobj);
        const char **lst = jsi_OptGetEnumList(spec);
        int i = 0;
        while (lst[i]) {
            Jsi_ObjArrayAdd(interp, sobj, Jsi_ValueNewStringKey(interp, lst[i]));
            i++;
        }
        Jsi_ObjInsert(interp, nobj, (spec->custom == Jsi_Opt_SwitchBitset?"bitSet":"enumList"), svalue, 0);
    } else if (spec->custom == Jsi_Opt_SwitchSuboption) {
        Jsi_OptionSpec* subSpec = (Jsi_OptionSpec*)data;
        Jsi_Obj *sobj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
        Jsi_Value *svalue = Jsi_ValueMakeObject(interp, NULL, sobj);
        jsi_DumpOptionSpecs(interp, sobj, subSpec);
        Jsi_ObjInsert(interp, nobj, "subSpec", svalue, 0);
    }
}