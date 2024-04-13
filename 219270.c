    yaffs_dir_open_meta_cb(YaffsCacheObject * /*obj*/, YaffsCacheVersion *version, void *args) {
        dir_open_cb_args *cb_args = (dir_open_cb_args *) args;
        YaffsCacheChunk *chunk = version->ycv_header_chunk;
        TSK_INUM_T curr_inode = 0;
        uint32_t obj_id = chunk->ycc_obj_id;
        uint32_t chunk_id = chunk->ycc_chunk_id;
        uint32_t vnum = version->ycv_version;
        YaffsHeader *header = NULL;
        TSK_FS_NAME * fs_name;
        char *file_ext;
        char version_string[64]; // Allow a max of 64 bytes in the version string

        yaffscache_obj_id_and_version_to_inode(obj_id, vnum, &curr_inode);

        if (chunk_id != 0) {
            return TSK_ERR;
        }

        if (tsk_verbose)
            fprintf(stderr, "dir_open_find_children_cb: %08" PRIxINUM " -> %08" PRIx32 ":%d\n", cb_args->parent_addr, obj_id, vnum);


        if (yaffsfs_read_header(cb_args->yfs, &header, chunk->ycc_offset) != TSK_OK) {
            return TSK_ERR;
        }

        if ((fs_name = tsk_fs_name_alloc(YAFFSFS_MAXNAMLEN + 64, 0)) == NULL) {
            free(header);
            return TSK_ERR;
        }

        switch (obj_id) {
        case YAFFS_OBJECT_LOSTNFOUND:
            strncpy(fs_name->name, YAFFS_OBJECT_LOSTNFOUND_NAME,
                fs_name->name_size - 64);
            break;
        case YAFFS_OBJECT_UNLINKED:
            strncpy(fs_name->name, YAFFS_OBJECT_UNLINKED_NAME,
                fs_name->name_size - 64);
            break;
        case YAFFS_OBJECT_DELETED:
            strncpy(fs_name->name, YAFFS_OBJECT_DELETED_NAME,
                fs_name->name_size - 64);
            break;
        default:
            strncpy(fs_name->name, header->name, fs_name->name_size - 64);
            break;
        }
        fs_name->name[fs_name->name_size - 65] = 0;

        // Only put object/version string onto unallocated versions
        if(! yaffs_is_version_allocated(cb_args->yfs, curr_inode)){ 
            // Also copy the extension so that it also shows up after the version string, which allows
            // easier searching by file extension. Max extension length is 5 characters after the dot,
            // and require at least one character before the dot
            file_ext = strrchr(fs_name->name, '.');
            if((file_ext != NULL) && (file_ext != fs_name->name) && (strlen(file_ext) < 7)){
               snprintf(version_string, 64, "#%d,%d%s", obj_id, vnum, file_ext);
            }
            else{
               snprintf(version_string, 64, "#%d,%d", obj_id, vnum);
            }
            strncat(fs_name->name, version_string, 64);
            fs_name->flags = TSK_FS_NAME_FLAG_UNALLOC;
        }
        else{
            fs_name->flags = TSK_FS_NAME_FLAG_ALLOC;
        }

        fs_name->meta_addr = curr_inode;

        switch (header->obj_type) {
        case YAFFS_TYPE_FILE:
            fs_name->type = TSK_FS_NAME_TYPE_REG;
            break;

        case YAFFS_TYPE_DIRECTORY:
            fs_name->type = TSK_FS_NAME_TYPE_DIR;
            break;

        case YAFFS_TYPE_SOFTLINK:
        case YAFFS_TYPE_HARDLINK:
            fs_name->type = TSK_FS_NAME_TYPE_LNK;
            break;

        case YAFFS_TYPE_SPECIAL:
            fs_name->type = TSK_FS_NAME_TYPE_UNDEF; // Could be a socket
            break;

        default:
            if (tsk_verbose)
                fprintf(stderr, "yaffs_dir_open_meta_cb: unhandled object type\n");
            fs_name->type = TSK_FS_NAME_TYPE_UNDEF;
            break;
        }

        free(header);

        if (tsk_fs_dir_add(cb_args->dir, fs_name)) {
            tsk_fs_name_free(fs_name);
            return TSK_ERR;
        }

        /* A copy is made in tsk_fs_dir_add, so we can free this one */
        tsk_fs_name_free(fs_name);

        return TSK_OK;
}