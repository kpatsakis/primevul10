    yaffsfs_close(TSK_FS_INFO *fs)
{
    if(fs != NULL){
        YAFFSFS_INFO *yfs = (YAFFSFS_INFO *)fs;

        fs->tag = 0;

        // Walk and free the cache structures
        yaffscache_objects_free(yfs);
        yaffscache_chunks_free(yfs);

        //tsk_deinit_lock(&yaffsfs->lock);
        tsk_fs_free(fs);
	}
}