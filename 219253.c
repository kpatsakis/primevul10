    yaffscache_version_find_by_inode(YAFFSFS_INFO *yfs, TSK_INUM_T inode, YaffsCacheVersion **version, YaffsCacheObject **obj_ret) {
        uint32_t obj_id, version_num;
        YaffsCacheObject *obj;
        YaffsCacheVersion *curr;

        if (version == NULL) {
            return TSK_ERR;
        }

        // convert inode to obj and version and find it in cache
        if (yaffscache_inode_to_obj_id_and_version(inode, &obj_id, &version_num) != TSK_OK) {
            *version = NULL;
            return TSK_ERR;
        }

        if (yaffscache_object_find(yfs, obj_id, &obj) != TSK_OK) {
            *version = NULL;
            return TSK_ERR;
        }

        if (version_num == 0) {
            if (obj_ret != NULL) {
                *obj_ret = obj;
            }
            *version = obj->yco_latest;
            return TSK_OK;
        }

        // Find the requested version in the list. 
        for(curr = obj->yco_latest; curr != NULL; curr = curr->ycv_prior) {
            if (curr->ycv_version == version_num) {
                if (obj_ret != NULL) {
                    *obj_ret = obj;
                }
                *version = curr;
                return TSK_OK;
            }
        }

        if (obj_ret != NULL) {
            *obj_ret = NULL;
        }
        *version = NULL;
        return TSK_ERR;
}