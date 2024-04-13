find_with_resource_array(WinLibrary *fi, WinResource *wr, const char *id)
{
    int c, rescnt;
    WinResource *return_wr;

    wr = list_resources(fi, wr, &rescnt);
    if (wr == NULL)
        return NULL;

    for (c = 0 ; c < rescnt ; c++) {
        if (compare_resource_id (&wr[c], id)) {
            /* duplicate WinResource and return it */
            return_wr = xmalloc(sizeof(WinResource));
            memcpy(return_wr, &wr[c], sizeof(WinResource));

            /* free old WinResource */
            free(wr);
            return return_wr;
        }
    }

    return NULL;
}