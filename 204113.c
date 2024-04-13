list_resources (WinLibrary *fi, WinResource *res, int *count)
{
    if (res != NULL && !res->is_directory)
        return NULL;

    if (fi->is_PE_binary) {
        return list_pe_resources(fi, (Win32ImageResourceDirectory *)
                 (res == NULL ? fi->first_resource : res->children),
                 (res == NULL ? 0 : res->level+1),
                 count);
    } else {
        return (res == NULL
                ? list_ne_type_resources(fi, count)
                : list_ne_name_resources(fi, res, count));
    }
}