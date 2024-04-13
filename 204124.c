get_resource_entry (WinLibrary *fi, WinResource *wr, size_t *size)
{
    if (fi->is_PE_binary) {
        Win32ImageResourceDataEntry *dataent;

        dataent = (Win32ImageResourceDataEntry *) wr->children;
        RETURN_IF_BAD_POINTER(NULL, *dataent);
        *size = dataent->size;
        RETURN_IF_BAD_OFFSET(NULL, fi->memory + dataent->offset_to_data, *size);

        return fi->memory + dataent->offset_to_data;
    } else {
        Win16NENameInfo *nameinfo;
        int sizeshift;

        nameinfo = (Win16NENameInfo *) wr->children;
        sizeshift = *((uint16_t *) fi->first_resource - 1);
        *size = nameinfo->length << sizeshift;
        RETURN_IF_BAD_OFFSET(NULL, fi->memory + (nameinfo->offset << sizeshift), *size);

        return fi->memory + (nameinfo->offset << sizeshift);
    }
}