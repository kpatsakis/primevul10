    yaffsfs_read_spare(YAFFSFS_INFO *yfs, YaffsSpare ** spare, TSK_OFF_T offset)
{
    unsigned char *spr;
    ssize_t cnt;
    YaffsSpare *sp;
    TSK_FS_INFO *fs = &(yfs->fs_info);

    uint32_t seq_number;
    uint32_t object_id;
    uint32_t chunk_id;

    // Should have checked this by now, but just in case
    if((yfs->spare_seq_offset + 4 > yfs->spare_size) ||
        (yfs->spare_obj_id_offset + 4 > yfs->spare_size) ||
        (yfs->spare_chunk_id_offset + 4 > yfs->spare_size)){
            return 1;
    }

    if ((spr = (unsigned char*) tsk_malloc(yfs->spare_size)) == NULL) {
        return 1;
    }

    if (yfs->spare_size < 46) { // Why is this 46?
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_ARG);
        tsk_error_set_errstr("yaffsfs_read_spare: spare size is too small");
        free(spr);
        return 1;
    }

    cnt = tsk_img_read(fs->img_info, offset, (char*) spr, yfs->spare_size);
    if ((cnt < 0) || ((unsigned int)cnt < yfs->spare_size)) {
        // couldn't read sufficient bytes...
        if (spare) {
            free(spr);
            *spare = NULL;
        }
        return 1;
    }

    if ((sp = (YaffsSpare*) tsk_malloc(sizeof(YaffsSpare))) == NULL) {
        return 1;
    }

    memset(sp, 0, sizeof(YaffsSpare));

    /*
    * Complete read of the YAFFS2 spare
    */


    // The format of the spare area should have been determined earlier
    memcpy(&seq_number, &spr[yfs->spare_seq_offset], 4);
    memcpy(&object_id, &spr[yfs->spare_obj_id_offset], 4);
    memcpy(&chunk_id, &spr[yfs->spare_chunk_id_offset], 4);

    if ((YAFFS_SPARE_FLAGS_IS_HEADER & chunk_id) != 0) {

        sp->seq_number = seq_number;
        sp->object_id = object_id & ~YAFFS_SPARE_OBJECT_TYPE_MASK;
        sp->chunk_id = 0;

        sp->has_extra_fields = 1;
        sp->extra_parent_id = chunk_id & YAFFS_SPARE_PARENT_ID_MASK;
        sp->extra_object_type =
            (object_id & YAFFS_SPARE_OBJECT_TYPE_MASK)
            >> YAFFS_SPARE_OBJECT_TYPE_SHIFT;
    }
    else {
        sp->seq_number = seq_number;
        sp->object_id = object_id;
        sp->chunk_id = chunk_id;

        sp->has_extra_fields = 0;
    }

    free(spr);
    *spare = sp;

    return 0;
}