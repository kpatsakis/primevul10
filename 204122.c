list_ne_name_resources (WinLibrary *fi, WinResource *typeres, int *count)
{
    int c, rescnt;
    WinResource *wr;
    Win16NETypeInfo *typeinfo = (Win16NETypeInfo *) typeres->this;
    Win16NENameInfo *nameinfo = (Win16NENameInfo *) typeres->children;

    /* count number of `type' resources */
    RETURN_IF_BAD_POINTER(NULL, typeinfo->count);
    *count = rescnt = typeinfo->count;
    if (rescnt == 0) return NULL;

    /* allocate WinResource's */
    wr = xmalloc(sizeof(WinResource) * rescnt);

    /* fill in the WinResource's */
    for (c = 0 ; c < rescnt ; c++) {
        RETURN_IF_BAD_POINTER(NULL, nameinfo[c]);
        wr[c].this = nameinfo+c;
        wr[c].is_directory = false;
        wr[c].children = nameinfo+c;
        wr[c].level = 1;

        /* fill in wr->id, wr->numeric_id */
        if (!decode_ne_resource_id (fi, wr + c, (nameinfo+c)->id)) {
            free(wr);
            return NULL;
        }
    }

    return wr;
}