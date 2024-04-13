    yaffsfs_read_header(YAFFSFS_INFO *yfs, YaffsHeader ** header, TSK_OFF_T offset)
{
    unsigned char *hdr;
    ssize_t cnt;
    YaffsHeader *head;
    TSK_FS_INFO *fs = &(yfs->fs_info);

    if ((hdr = (unsigned char*) tsk_malloc(yfs->page_size)) == NULL) {
        return 1;
    }

    cnt = tsk_img_read(fs->img_info, offset, (char *) hdr,
        yfs->page_size);
    if ((cnt < 0) || ((unsigned int)cnt < yfs->page_size)) {
        free(hdr);
        return 1;
    }

    if ((head = (YaffsHeader*) tsk_malloc( sizeof(YaffsHeader))) == NULL) {
        free(hdr);
        return 1;
    }

    memcpy(&head->obj_type, hdr, 4);
    memcpy(&head->parent_id, &hdr[4], 4);
    memcpy(head->name, (char*) &hdr[0xA], YAFFS_HEADER_NAME_LENGTH);
    memcpy(&head->file_mode, &hdr[0x10C], 4);
    memcpy(&head->user_id, &hdr[0x110], 4);
    memcpy(&head->group_id, &hdr[0x114], 4);
    memcpy(&head->atime, &hdr[0x118], 4);
    memcpy(&head->mtime, &hdr[0x11C], 4);
    memcpy(&head->ctime, &hdr[0x120], 4);
    memcpy(&head->file_size, &hdr[0x124], 4);
    memcpy(&head->equivalent_id, &hdr[0x128], 4);
    memcpy(head->alias, (char*) &hdr[0x12C], YAFFS_HEADER_ALIAS_LENGTH);

    //memcpy(&head->rdev_mode, &hdr[0x1CC], 4);
    //memcpy(&head->win_ctime, &hdr[0x1D0], 8);
    //memcpy(&head->win_atime, &hdr[0x1D8], 8);
    //memcpy(&head->win_mtime, &hdr[0x1E0], 8);
    //memcpy(&head->inband_obj_id, &hdr[0x1E8], 4);
    //memcpy(&head->inband_is_shrink, &hdr[0x1EC], 4);

    // NOTE: This isn't in Android 3.3 kernel but is in YAFFS2 git
    //memcpy(&head->file_size_high, &hdr[0x1F0], 4);

    free(hdr);

    *header = head;
    return 0;
}