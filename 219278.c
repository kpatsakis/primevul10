    yaffscache_inode_to_obj_id_and_version(TSK_INUM_T inode, uint32_t *obj_id, uint32_t *version_num) {
        *obj_id = inode & YAFFS_OBJECT_ID_MASK;
        *version_num = (inode >> YAFFS_VERSION_NUM_SHIFT) & YAFFS_VERSION_NUM_MASK;

        return TSK_OK;
}