static uint8_t yaffs_is_version_allocated(YAFFSFS_INFO * yfs, TSK_INUM_T inode){
    YaffsCacheObject * obj;
    YaffsCacheVersion * version;
    YaffsCacheChunk * curr;

    TSK_RETVAL_ENUM result = yaffscache_version_find_by_inode(yfs, inode, &version, &obj);
    if (result != TSK_OK) {
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffs_is_version_allocated: yaffscache_version_find_by_inode failed! (inode: %d)\n", inode);
        return 0;
    }

    if(obj->yco_latest == version){
        curr = obj->yco_latest->ycv_header_chunk;
        while(curr != NULL){
            // We're looking for a newer unlinked or deleted header. If one exists, then this object should be considered unallocated
            if((curr->ycc_parent_id == YAFFS_OBJECT_UNLINKED) || (curr->ycc_parent_id == YAFFS_OBJECT_DELETED)){
                return 0;
            }
            curr = curr ->ycc_next;
        }
        return 1;
    }
    else{
        return 0;
    }

}