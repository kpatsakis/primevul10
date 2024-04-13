list_pe_resources (WinLibrary *fi, Win32ImageResourceDirectory *pe_res, int level, int *count)
{
    WinResource *wr;
    unsigned int out_c;
    int dirent_c, rescnt;
    Win32ImageResourceDirectoryEntry *dirent
      = (Win32ImageResourceDirectoryEntry *) (pe_res + 1);

    /* count number of `type' resources */
    RETURN_IF_BAD_POINTER(NULL, *dirent);
    rescnt = pe_res->number_of_named_entries + pe_res->number_of_id_entries;
    *count = 0;
    if (rescnt == 0) return NULL;

    /* allocate WinResource's */
    wr = xmalloc(sizeof(WinResource) * rescnt);

    /* fill in the WinResource's */
    out_c = 0;
    for (dirent_c = 0 ; dirent_c < rescnt ; dirent_c++) {
        RETURN_IF_BAD_POINTER(NULL, dirent[dirent_c]);
        wr[out_c].this = pe_res;
        wr[out_c].level = level;
        wr[out_c].is_directory = (dirent[dirent_c].u2.s.data_is_directory);
        /* Require data to point somewhere after the directory */
        if (dirent[dirent_c].u2.s.offset_to_directory < sizeof(Win32ImageResourceDirectory))
            continue;
        wr[out_c].children = fi->first_resource + dirent[dirent_c].u2.s.offset_to_directory;

        /* fill in wr->id, wr->numeric_id */
        if (!decode_pe_resource_id (fi, wr + out_c, dirent[dirent_c].u1.name))
            continue;

        ++out_c;
        ++(*count);
    }
    if (out_c == 0) {
        free(wr);
        return NULL;
    }

    return wr;
}