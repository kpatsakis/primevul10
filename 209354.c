bool Jsi_OptionsValid(Jsi_Interp *interp,  Jsi_OptionSpec* spec)
{
    int i = 0;
    while (spec[i].id>=JSI_OPTION_BOOL && spec[i].id < JSI_OPTION_END) {
        SIGASSERTMASK(spec+i, OPTS, 0xff);
        if (spec[i].help && Jsi_Strchr(spec[i].help, '\n')) {
            if (interp)
                Jsi_LogError("item \"%s\": help contains newline", spec[i].name);
            return 0;
        }
        i++;
    }
    return (i>0);
}