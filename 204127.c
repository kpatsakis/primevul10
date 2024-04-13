calc_vma_size (WinLibrary *fi)
{
    Win32ImageSectionHeader *seg;
    size_t c, segcount, size;

    size = 0;
    RETURN_IF_BAD_POINTER(-1, PE_HEADER(fi->memory)->file_header.number_of_sections);
    segcount = PE_HEADER(fi->memory)->file_header.number_of_sections;

    /* If there are no segments, just process file like it is.
     * This is (probably) not the right thing to do, but problems
     * will be delt with later anyway.
     */
    if (segcount == 0)
        return fi->total_size;

    RETURN_IF_BAD_PE_SECTIONS(-1, fi->memory);
    seg = PE_SECTIONS(fi->memory);
    RETURN_IF_BAD_POINTER(-1, *seg);
    
    for (c = 0 ; c < segcount ; c++) {
        RETURN_IF_BAD_POINTER(0, *seg);

        size = MAX(size, seg->virtual_address + seg->size_of_raw_data);
        /* I have no idea what misc.virtual_size is for... */
        size = MAX(size, seg->virtual_address + seg->misc.virtual_size);
        seg++;
    }

    return size;
}