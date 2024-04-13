list_ne_type_resources (WinLibrary *fi, int *count)
{
    size_t c, rescnt;
    WinResource *wr;
    Win16NETypeInfo *typeinfo;

    /* count number of `type' resources */
    typeinfo = (Win16NETypeInfo *) fi->first_resource;
    RETURN_IF_BAD_POINTER(NULL, *typeinfo);
    for (rescnt = 0 ; typeinfo->type_id != 0 ; rescnt++) {
        if (((char *) NE_TYPEINFO_NEXT(typeinfo))+sizeof(uint16_t) > fi->memory + fi->total_size) {
            warn(_("%s: resource table invalid, ignoring remaining entries"), fi->name);
            break;
        }
        typeinfo = NE_TYPEINFO_NEXT(typeinfo);
        RETURN_IF_BAD_POINTER(NULL, *typeinfo);
    }
    *count = rescnt;
    if (rescnt == 0) return NULL;

    /* allocate WinResource's */
    wr = xmalloc(sizeof(WinResource) * rescnt);

    /* fill in the WinResource's */
    typeinfo = (Win16NETypeInfo *) fi->first_resource;
    for (c = 0 ; c < rescnt ; c++) {
        wr[c].this = typeinfo;
        wr[c].is_directory = (typeinfo->count != 0);
        wr[c].children = typeinfo+1;
        wr[c].level = 0;

        /* fill in wr->id, wr->numeric_id */
        if (!decode_ne_resource_id (fi, wr + c, typeinfo->type_id)) {
            free(wr);
            return NULL;
        }

        typeinfo = NE_TYPEINFO_NEXT(typeinfo);
    }

    return wr;
}