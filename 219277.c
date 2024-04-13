    yaffsfs_fsstat(TSK_FS_INFO * fs, FILE * hFile)
{
    YAFFSFS_INFO *yfs = (YAFFSFS_INFO *) fs;
    unsigned int obj_count, version_count;
    uint32_t obj_first, obj_last, version_first, version_last;

    // clean up any error messages that are lying around
    tsk_error_reset();

    tsk_fprintf(hFile, "FILE SYSTEM INFORMATION\n");
    tsk_fprintf(hFile, "--------------------------------------------\n");

    tsk_fprintf(hFile, "File System Type: YAFFS2\n");
    tsk_fprintf(hFile, "Page Size: %u\n", yfs->page_size);
    tsk_fprintf(hFile, "Spare Size: %u\n", yfs->spare_size);
    tsk_fprintf(hFile, "Spare Offsets: Sequence number: %d, Object ID: %d, Chunk ID: %d, nBytes: %d\n",
        yfs->spare_seq_offset, yfs->spare_obj_id_offset, yfs->spare_chunk_id_offset, yfs->spare_nbytes_offset);

    tsk_fprintf(hFile, "\nMETADATA INFORMATION\n");
    tsk_fprintf(hFile, "--------------------------------------------\n");


    yaffscache_objects_stats(yfs, 
        &obj_count, &obj_first, &obj_last,
        &version_count, &version_first, &version_last);

    tsk_fprintf(hFile, "Number of Allocated Objects: %u\n", obj_count);
    tsk_fprintf(hFile, "Object Id Range: %" PRIu32 " - %" PRIu32 "\n",
        obj_first, obj_last);
    tsk_fprintf(hFile, "Number of Total Object Versions: %u\n", version_count);
    tsk_fprintf(hFile, "Object Version Range: %" PRIu32 " - %" PRIu32 "\n",
        version_first, version_last);

    return 0;
}