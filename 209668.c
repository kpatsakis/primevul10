const Jsi_OptionTypedef* Jsi_OptionTypeInfo(Jsi_OptionId typ) {
    if (typ>=JSI_OPTION_BOOL && typ < JSI_OPTION_END)
        return jsi_OptTypeInfo+typ;
    return NULL;
}