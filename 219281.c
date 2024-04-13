    yaffscache_find_children(YAFFSFS_INFO *yfs, TSK_INUM_T parent_inode, yc_find_children_cb cb, void *args)
{
    YaffsCacheObject *obj;

    uint32_t parent_id, version_num;
    if (yaffscache_inode_to_obj_id_and_version(parent_inode, &parent_id, &version_num) != TSK_OK) {
        return TSK_ERR;
    }

    /* Iterate over all objects and all versions of the objects to see if one is the child
     * of the given parent. */
    for (obj = yfs->cache_objects; obj != NULL; obj = obj->yco_next) {
        YaffsCacheVersion *version;
        for (version = obj->yco_latest; version != NULL; version = version->ycv_prior) {
            /* Is this an incomplete version? */
            if (version->ycv_header_chunk == NULL) {
                continue;
            }

            if (version->ycv_header_chunk->ycc_parent_id == parent_id) {
                TSK_RETVAL_ENUM result = cb(obj, version, args);
                if (result != TSK_OK)
                    return result;
            }
        }
    }

    return TSK_OK;
}