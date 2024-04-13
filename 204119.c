get_data_directory_entry (WinLibrary *fi, unsigned int entry)
{
    Win32ImageNTHeaders *pe_header;
    pe_header = PE_HEADER(fi->memory);
    RETURN_IF_BAD_POINTER(NULL, pe_header->optional_header.magic);

    if (pe_header->optional_header.magic == OPTIONAL_MAGIC_PE32) {
        Win32ImageOptionalHeader *optional_header = &(pe_header->optional_header);
        RETURN_IF_BAD_POINTER(false, optional_header->data_directory[entry]);
        return optional_header->data_directory + entry;
    } else if (pe_header->optional_header.magic == OPTIONAL_MAGIC_PE32_64) {
        Win32ImageOptionalHeader64 *optional_header =
            (Win32ImageOptionalHeader64*)&(pe_header->optional_header);
        RETURN_IF_BAD_POINTER(false, optional_header->data_directory[entry]);
        return optional_header->data_directory + entry;
    } else {
        return NULL;
    }
}