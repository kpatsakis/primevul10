static void jsi_DumpOptionSpec(Jsi_Interp *interp, Jsi_Obj *nobj, Jsi_OptionSpec* spec, int addName)
{
    if (addName)
        Jsi_ObjInsert(interp, nobj, "name", Jsi_ValueNewStringKey(interp, spec->name),0);
    if (spec->help) {
        if (Jsi_Strchr(spec->help, '\n'))
            Jsi_LogError("%s .help contains newline: %s", spec->name, spec->help);
        Jsi_ObjInsert(interp, nobj, "help", Jsi_ValueNewStringKey(interp, spec->help),0);
    }
    if (spec->info)
        Jsi_ObjInsert(interp, nobj, "info", Jsi_ValueNewStringKey(interp, spec->info),0);
    Jsi_ObjInsert(interp, nobj, "type", Jsi_ValueNewStringKey(interp, jsi_OptionTypeStr(spec->id,0)),0);
    Jsi_ObjInsert(interp, nobj, "cName", Jsi_ValueNewStringKey(interp, jsi_OptionTypeStr(spec->id,1)),0);
    Jsi_ObjInsert(interp, nobj, "initOnly", Jsi_ValueNewBoolean(interp, (spec->flags & JSI_OPT_INIT_ONLY)!=0), 0);
    Jsi_ObjInsert(interp, nobj, "readOnly", Jsi_ValueNewBoolean(interp, (spec->flags & JSI_OPT_READ_ONLY)!=0), 0);
    Jsi_ObjInsert(interp, nobj, "required", Jsi_ValueNewBoolean(interp, (spec->flags & JSI_OPT_REQUIRED)!=0), 0);
    Jsi_ObjInsert(interp, nobj, "noCase", Jsi_ValueNewBoolean(interp, (spec->flags & JSI_OPT_CUST_NOCASE)!=0), 0);
    Jsi_ObjInsert(interp, nobj, "size", Jsi_ValueNewNumber(interp, (Jsi_Number)spec->size), 0);
    if (spec->flags)
        Jsi_ObjInsert(interp, nobj, "flags", Jsi_ValueNewNumber(interp, (Jsi_Number)spec->flags), 0);
    if (spec->data) {
        if (spec->id == JSI_OPTION_FUNC && spec->data) 
            Jsi_ObjInsert(interp, nobj, "args", Jsi_ValueNewStringDup(interp, (char*)spec->data), 0);
        else if (spec->id == JSI_OPTION_CUSTOM && (spec->custom == Jsi_Opt_SwitchBitset ||
            spec->custom == Jsi_Opt_SwitchEnum)) {
            const char **list = jsi_OptGetEnumList(spec);
            if (list)
                Jsi_ObjInsert(interp, nobj, "data", Jsi_ValueNewArray(interp, list, -1), 0);
        }
    }
    if (spec->id == JSI_OPTION_CUSTOM && spec->custom) {
        Jsi_Obj *sobj = Jsi_ObjNewType(interp, JSI_OT_OBJECT);
        Jsi_Value *cvalue = Jsi_ValueMakeObject(interp, NULL, sobj);
        jsi_DumpCustomSpec(interp, sobj,  spec);
        Jsi_ObjInsert(interp, nobj, "customArg", cvalue,0);
        Jsi_OptionSpec *os = spec;
        while (os->id != JSI_OPTION_END) os++;
        Jsi_ObjInsert(interp, nobj, "customArgHelp", Jsi_ValueNewStringKey(interp, (os->help?os->help:"")), 0);
        
    }
}