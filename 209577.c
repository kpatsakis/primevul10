void jsi_DumpOptionSpecs(Jsi_Interp *interp, Jsi_Obj *nobj, Jsi_OptionSpec* spec)
{
    int i = 0;
    while (spec[i].id>=JSI_OPTION_BOOL && spec[i].id < JSI_OPTION_END) {
        Jsi_Obj *sobj = Jsi_ObjNewType(interp, JSI_OT_OBJECT);
        Jsi_Value *svalue = Jsi_ValueMakeObject(interp, NULL, sobj);
        jsi_DumpOptionSpec(interp, sobj, spec+i, 1);
        Jsi_ObjArrayAdd(interp, nobj, svalue);
        i++;
    }
}