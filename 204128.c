do_resources (WinLibrary *fi, const char *type, const char *name, const char *lang, DoResourceCallback cb)
{
    WinResource *type_wr;
    WinResource *name_wr;
    WinResource *lang_wr;

    type_wr = malloc(sizeof(WinResource)*3);
    name_wr = type_wr + 1;
    lang_wr = type_wr + 2;
    memset(type_wr, 0, sizeof(WinResource)*3);

    do_resources_recurs(fi, NULL, type_wr, name_wr, lang_wr, type, name, lang, cb);

    free(type_wr);
}