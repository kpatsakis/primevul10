    yaffscache_obj_id_and_version_to_inode(uint32_t obj_id, uint32_t version_num, TSK_INUM_T *inode) {
        if ((obj_id & ~YAFFS_OBJECT_ID_MASK) != 0) {
            tsk_error_reset();
            tsk_error_set_errno(TSK_ERR_FS);
            tsk_error_set_errstr(
                "yaffsfs_parse_image_load_cache: Max object ID %" PRIu32 " is invalid", obj_id);
            return TSK_ERR;
        }

        if ((version_num & ~YAFFS_VERSION_NUM_MASK) != 0) {
            tsk_error_reset();
            tsk_error_set_errno(TSK_ERR_FS);
            tsk_error_set_errstr(
                "yaffsfs_parse_image_load_cache: Max version number %" PRIu32 " is invalid", version_num);
            return TSK_ERR;
        }

        *inode = obj_id | (version_num << YAFFS_VERSION_NUM_SHIFT);
        return TSK_OK;
}