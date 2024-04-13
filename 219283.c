    yaffsfs_read_chunk(YAFFSFS_INFO *yfs,
    YaffsHeader **header, YaffsSpare **spare, TSK_OFF_T offset)
{
    TSK_OFF_T header_offset = offset;
    TSK_OFF_T spare_offset = offset + yfs->page_size; 

    if (header == NULL || spare == NULL) {
        return 1;
    }

    if (yaffsfs_read_header(yfs, header, header_offset) != 0) {
        return 1;
    }

    if (yaffsfs_read_spare(yfs, spare, spare_offset) != 0) {
        free(*header);
        *header = NULL;
        return 1;
    }

    return 0;
}