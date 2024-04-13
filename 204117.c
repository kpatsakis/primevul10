do_resources_recurs (WinLibrary *fi, WinResource *base, WinResource *type_wr,
                     WinResource *name_wr, WinResource *lang_wr,
                     const char *type, const char *name, const char *lang, DoResourceCallback cb)
{
    int c, rescnt;
    WinResource *wr;

    /* get a list of all resources at this level */
    wr = list_resources (fi, base, &rescnt);
    if (wr == NULL)
        return;

    /* process each resource listed */
    for (c = 0 ; c < rescnt ; c++) {
        /* (over)write the corresponding WinResource holder with the current */
        memcpy(WINRESOURCE_BY_LEVEL(wr[c].level), wr+c, sizeof(WinResource));
        if ((base && (wr[c].level <= base->level))
            || (wr[c].level >= 3))
        {
            warn(_("%s: resource structure malformed"), fi->name);
            return;
        }

        /* go deeper unless there is something that does NOT match */
        if (LEVEL_MATCHES(type) && LEVEL_MATCHES(name) && LEVEL_MATCHES(lang)) {
            if (wr->is_directory)
                do_resources_recurs (fi, wr+c, type_wr, name_wr, lang_wr, type, name, lang, cb);
            else
                cb(fi, wr+c, type_wr, name_wr, lang_wr);
        }
    }

    /* since we're moving back one level after this, unset the
     * WinResource holder used on this level */
    memset(WINRESOURCE_BY_LEVEL(wr[0].level), 0, sizeof(WinResource));
}