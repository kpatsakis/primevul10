extract_bitmap_resource(WinLibrary *fi, WinResource *wr, int *ressize)
{
    Win32BitmapInfoHeader info;
    uint8_t *result;
    uint8_t *resentry;
    uint32_t offbits;
    int size;

    resentry=(uint8_t *)(get_resource_entry(fi,wr,&size));

    /* Bitmap file consists of:
     * 1) File header (14 bytes)
     * 2) Bitmap header (40 bytes)
     * 3) Colormap (size depends on a few things)
     * 4) Pixels data
     *
     * parts 2-4 are present in the resource data, we need just
     * to add a file header, which contains file size and offset
     * from file beginning to pixels data.
     */

    /* Get the bitmap info */
    memcpy(&info,resentry,sizeof(info));
    fix_win32_bitmap_info_header_endian(&info);

    /* offbits - offset from file start to the beginning
     *           of the first pixel data */
    offbits = info.size+14;

    /* In 24-bit bitmaps there's no colormap
     * The size of an entry in colormap is 4
     */
    if (info.bit_count!=24) {

        /* 0 value of clr_used means that all possible color
        * entries are used */
       if (info.clr_used==0) {
           switch (info.bit_count) {
               case 1:    /* Monochrome bitmap */
                   offbits += 8;
                   break;
               case 4:    /* 16 colors bitmap */
                   offbits += 64;
                   break;
               case 8:    /* 256 colors bitmap */
                  offbits += 1024;
           }
       } else {
           offbits += 4 * info.clr_used;
       }
    }

    /* The file will consist of the resource data and
     * 14 bytes long file header */
    *ressize = 14+size;
    result = (uint8_t *)xmalloc(*ressize);

    /* Filling the file header with data */
    result[0] = 'B';   /* Magic char #1 */
    result[1] = 'M';   /* Magic char #2 */
    result[2] = (*ressize & 0x000000ff);      /* file size, little-endian */
    result[3] = (*ressize & 0x0000ff00)>>8;
    result[4] = (*ressize & 0x00ff0000)>>16;
    result[5] = (*ressize & 0xff000000)>>24;
    result[6] = 0; /* Reserved */
    result[7] = 0;
    result[8] = 0;
    result[9] = 0;
    result[10] = (offbits & 0x000000ff);  /* offset to pixels, little-endian */
    result[11] = (offbits & 0x0000ff00)>>8;
    result[12] = (offbits & 0x00ff0000)>>16;
    result[13] = (offbits & 0xff000000)>>24;

    /* The rest of the file is the resource entry */
    memcpy(result+14,resentry,size);

    return result;
}