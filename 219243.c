    yaffs2_open(TSK_IMG_INFO * img_info, TSK_OFF_T offset,
    TSK_FS_TYPE_ENUM ftype, uint8_t test)
{
    YAFFSFS_INFO *yaffsfs = NULL;
    TSK_FS_INFO *fs = NULL;
    const unsigned int psize = img_info->page_size;
    const unsigned int ssize = img_info->spare_size;
    YaffsHeader * first_header = NULL;
    TSK_FS_DIR *test_dir;
    std::map<std::string, std::string> configParams;
    YAFFS_CONFIG_STATUS config_file_status;

    // clean up any error messages that are lying around
    tsk_error_reset();

    if (TSK_FS_TYPE_ISYAFFS2(ftype) == 0) {
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_ARG);
        tsk_error_set_errstr("Invalid FS Type in yaffsfs_open");
        return NULL;
    }

    if (img_info->sector_size == 0) {
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_ARG);
        tsk_error_set_errstr("yaffs2_open: sector size is 0");
        return NULL;
    }

    

    if ((yaffsfs = (YAFFSFS_INFO *) tsk_fs_malloc(sizeof(YAFFSFS_INFO))) == NULL)
        return NULL;
    yaffsfs->cache_objects = NULL;
    yaffsfs->chunkMap = NULL;

    fs = &(yaffsfs->fs_info);

    fs->tag = TSK_FS_INFO_TAG;
    fs->ftype = ftype;
    fs->flags = (TSK_FS_INFO_FLAG_ENUM)0;
    fs->img_info = img_info;
    fs->offset = offset;
    fs->endian = TSK_LIT_ENDIAN;

    // Read config file (if it exists)
    config_file_status = yaffs_load_config_file(img_info, configParams);
    // BL-6929(JTS): When using external readers, this call will fail.
    // Not having a config should not be a fatal error.
  /*if(config_file_status == YAFFS_CONFIG_ERROR){
        // tsk_error was set by yaffs_load_config
        goto on_error;
    }
    else*/ if(config_file_status == YAFFS_CONFIG_OK){
        // Validate the input
        // If it fails validation, return (tsk_error will be set up already)
        if(1 == yaffs_validate_config_file(configParams)){
            goto on_error;
        }
    }

    // If we read these fields from the config file, use those values. Otherwise use the defaults
    if(configParams.find(YAFFS_CONFIG_PAGE_SIZE_STR) != configParams.end()){
        yaffsfs->page_size = atoi(configParams[YAFFS_CONFIG_PAGE_SIZE_STR].c_str());
    }
    else{
        yaffsfs->page_size = psize == 0 ? YAFFS_DEFAULT_PAGE_SIZE : psize;
    }

    if(configParams.find(YAFFS_CONFIG_SPARE_SIZE_STR) != configParams.end()){
        yaffsfs->spare_size = atoi(configParams[YAFFS_CONFIG_SPARE_SIZE_STR].c_str());
    }
    else{
        yaffsfs->spare_size = ssize == 0 ? YAFFS_DEFAULT_SPARE_SIZE : ssize;
    }

    if(configParams.find(YAFFS_CONFIG_CHUNKS_PER_BLOCK_STR) != configParams.end()){
        yaffsfs->chunks_per_block = atoi(configParams[YAFFS_CONFIG_CHUNKS_PER_BLOCK_STR].c_str());
    }
    else{
        yaffsfs->chunks_per_block = 64;
    }

    // TODO: Why are 2 different memory allocation methods used in the same code?
    // This makes things unnecessary complex.
    yaffsfs->max_obj_id = 1;
    yaffsfs->max_version = 0;

    // Keep track of whether we're doing auto-detection of the file system
    if(test){
        yaffsfs->autoDetect = 1;
    }
    else{
        yaffsfs->autoDetect = 0;
    }

    // Determine the layout of the spare area
    // If it was specified in the config file, use those values. Otherwise do the auto-detection
    if(configParams.find(YAFFS_CONFIG_SEQ_NUM_STR) != configParams.end()){
        // In the validation step, we ensured that if one of the offsets was set, we have all of them
        yaffsfs->spare_seq_offset = atoi(configParams[YAFFS_CONFIG_SEQ_NUM_STR].c_str());
        yaffsfs->spare_obj_id_offset = atoi(configParams[YAFFS_CONFIG_OBJ_ID_STR].c_str());
        yaffsfs->spare_chunk_id_offset = atoi(configParams[YAFFS_CONFIG_CHUNK_ID_STR].c_str());

        // Check that the offsets are valid for the given spare area size (fields are 4 bytes long)
        if((yaffsfs->spare_seq_offset + 4 > yaffsfs->spare_size) ||
            (yaffsfs->spare_obj_id_offset + 4 > yaffsfs->spare_size) ||
            (yaffsfs->spare_chunk_id_offset + 4 > yaffsfs->spare_size)){
            tsk_error_reset();
            tsk_error_set_errno(TSK_ERR_FS);
            tsk_error_set_errstr("yaffs2_open: Offset(s) in config file too large for spare area (size %d). %s", yaffsfs->spare_size, YAFFS_HELP_MESSAGE);
            goto on_error;
        }


        // nBytes isn't currently used, so just set to zero
        yaffsfs->spare_nbytes_offset = 0;
    }
    else{
        // Decide how many blocks to test. If we're not doing auto-detection, set to zero (no limit)
        unsigned int maxBlocksToTest;
        if(yaffsfs->autoDetect){
            maxBlocksToTest = YAFFS_DEFAULT_MAX_TEST_BLOCKS;
        }
        else{
            maxBlocksToTest = 0;
        }

        if(yaffs_initialize_spare_format(yaffsfs, maxBlocksToTest) != TSK_OK){
            tsk_error_reset();
            tsk_error_set_errno(TSK_ERR_FS_MAGIC);
            tsk_error_set_errstr("not a YAFFS file system (bad spare format). %s", YAFFS_HELP_MESSAGE);
            if (tsk_verbose)
                fprintf(stderr, "yaffsfs_open: could not find valid spare area format\n%s\n", YAFFS_HELP_MESSAGE);
            goto on_error;
        }
    }

    /*
    * Read the first record, make sure it's a valid header...
    *
    * Used for verification and autodetection of
    * the FS type.
    */
    if (yaffsfs_read_header(yaffsfs, &first_header, 0)) {
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_MAGIC);
        tsk_error_set_errstr("not a YAFFS file system (first record). %s", YAFFS_HELP_MESSAGE);
        if (tsk_verbose)
            fprintf(stderr, "yaffsfs_open: invalid first record\n%s\n", YAFFS_HELP_MESSAGE);
        goto on_error;
    }
    free(first_header);
    first_header = NULL;

    fs->duname = "Chunk";

    /*
    * Calculate the meta data info
    */
    //fs->last_inum = 0xffffffff; // Will update this as we go
    fs->last_inum = 0;
    fs->root_inum = YAFFS_OBJECT_ROOT;
    fs->first_inum = YAFFS_OBJECT_FIRST;
    //fs->inum_count = fs->last_inum; // For now this will be the last_inum - 1 (after we calculate it)

    /*
    * Calculate the block info
    */
    fs->dev_bsize = img_info->sector_size;
    fs->block_size = yaffsfs->page_size;
    fs->block_pre_size = 0;
    fs->block_post_size = yaffsfs->spare_size;
    fs->block_count = img_info->size / (fs->block_pre_size + fs->block_size + fs->block_post_size);
    fs->first_block = 0;
    fs->last_block_act = fs->last_block = fs->block_count ? fs->block_count - 1 : 0;

    /* Set the generic function pointers */
    fs->inode_walk = yaffsfs_inode_walk;
    fs->block_walk = yaffsfs_block_walk;
    fs->block_getflags = yaffsfs_block_getflags;

    fs->get_default_attr_type = yaffsfs_get_default_attr_type;
    fs->load_attrs = yaffsfs_load_attrs;

    fs->file_add_meta = yaffs_inode_lookup;
    fs->dir_open_meta = yaffsfs_dir_open_meta;
    fs->fsstat = yaffsfs_fsstat;
    fs->fscheck = yaffsfs_fscheck;
    fs->istat = yaffsfs_istat;
    fs->name_cmp = tsk_fs_unix_name_cmp;

    fs->close = yaffsfs_close;

    /* Journal */
    fs->jblk_walk = yaffsfs_jblk_walk;
    fs->jentry_walk = yaffsfs_jentry_walk;
    fs->jopen = yaffsfs_jopen;

    /* Initialize the caches */
    if (tsk_verbose)
        fprintf(stderr, "yaffsfs_open: building cache...\n");

    /* Build cache */
    /* NOTE: The only modifications to the cache happen here, during at 
    *       the open. Should be fine with no lock, even if access to the
    *       cache is shared among threads.
    */
    //tsk_init_lock(&yaffsfs->lock);
    yaffsfs->chunkMap = new std::map<uint32_t, YaffsCacheChunkGroup>;
    if (TSK_OK != yaffsfs_parse_image_load_cache(yaffsfs)) {
        goto on_error;
    }

    if (tsk_verbose) {
        fprintf(stderr, "yaffsfs_open: done building cache!\n");
        //yaffscache_objects_dump(yaffsfs, stderr);
    }

    // Update the number of inums now that we've read in the file system
    fs->inum_count = fs->last_inum - 1;

    test_dir = tsk_fs_dir_open_meta(fs, fs->root_inum);
    if (test_dir == NULL) {
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_MAGIC);
        tsk_error_set_errstr("not a YAFFS file system (no root directory). %s", YAFFS_HELP_MESSAGE);
        if (tsk_verbose)
            fprintf(stderr, "yaffsfs_open: invalid file system\n%s\n", YAFFS_HELP_MESSAGE);
        goto on_error;
    }
    tsk_fs_dir_close(test_dir);

    return fs;

on_error:
    // yaffsfs_close frees all the cache objects
    yaffsfs_close(fs);

    return NULL;
}