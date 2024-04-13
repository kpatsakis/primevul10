static struct inode *read_inode(unsigned int start_block, unsigned int offset)
{
	static union squashfs_inode_header_1 header;
	long long start = sBlk.s.inode_table_start + start_block;
	int bytes = lookup_entry(inode_table_hash, start);
	char *block_ptr = inode_table + bytes + offset;
	static struct inode i;

	TRACE("read_inode: reading inode [%d:%d]\n", start_block,  offset);

	if(bytes == -1)
		EXIT_UNSQUASH("read_inode: inode table block %lld not found\n",
			 start); 

	if(swap) {
		squashfs_base_inode_header_1 sinode;
		memcpy(&sinode, block_ptr, sizeof(header.base));
		SQUASHFS_SWAP_BASE_INODE_HEADER_1(&header.base, &sinode,
			sizeof(squashfs_base_inode_header_1));
	} else
		memcpy(&header.base, block_ptr, sizeof(header.base));

	i.uid = (uid_t) uid_table[(header.base.inode_type - 1) /
		SQUASHFS_TYPES * 16 + header.base.uid];
	if(header.base.inode_type == SQUASHFS_IPC_TYPE) {
		squashfs_ipc_inode_header_1 *inodep = &header.ipc;

		if(swap) {
			squashfs_ipc_inode_header_1 sinodep;
			memcpy(&sinodep, block_ptr, sizeof(sinodep));
			SQUASHFS_SWAP_IPC_INODE_HEADER_1(inodep, &sinodep);
		} else
			memcpy(inodep, block_ptr, sizeof(*inodep));

		if(inodep->type == SQUASHFS_SOCKET_TYPE) {
			i.mode = S_IFSOCK | header.base.mode;
			i.type = SQUASHFS_SOCKET_TYPE;
		} else {
			i.mode = S_IFIFO | header.base.mode;
			i.type = SQUASHFS_FIFO_TYPE;
		}
		i.uid = (uid_t) uid_table[inodep->offset * 16 + inodep->uid];
	} else {
		i.mode = lookup_type[(header.base.inode_type - 1) %
			SQUASHFS_TYPES + 1] | header.base.mode;
		i.type = (header.base.inode_type - 1) % SQUASHFS_TYPES + 1;
	}

	i.xattr = SQUASHFS_INVALID_XATTR;
	i.gid = header.base.guid == 15 ? i.uid :
		(uid_t) guid_table[header.base.guid];
	i.time = sBlk.s.mkfs_time;
	i.inode_number = inode_number ++;

	switch(i.type) {
		case SQUASHFS_DIR_TYPE: {
			squashfs_dir_inode_header_1 *inode = &header.dir;

			if(swap) {
				squashfs_dir_inode_header_1 sinode;
				memcpy(&sinode, block_ptr, sizeof(header.dir));
				SQUASHFS_SWAP_DIR_INODE_HEADER_1(inode,
					&sinode);
			} else
			memcpy(inode, block_ptr, sizeof(header.dir));

			i.data = inode->file_size;
			i.offset = inode->offset;
			i.start = inode->start_block;
			i.time = inode->mtime;
			break;
		}
		case SQUASHFS_FILE_TYPE: {
			squashfs_reg_inode_header_1 *inode = &header.reg;

			if(swap) {
				squashfs_reg_inode_header_1 sinode;
				memcpy(&sinode, block_ptr, sizeof(sinode));
				SQUASHFS_SWAP_REG_INODE_HEADER_1(inode,
					&sinode);
			} else
				memcpy(inode, block_ptr, sizeof(*inode));

			i.data = inode->file_size;
			i.time = inode->mtime;
			i.blocks = (i.data + sBlk.s.block_size - 1) >>
				sBlk.s.block_log;
			i.start = inode->start_block;
			i.block_ptr = block_ptr + sizeof(*inode);
			i.fragment = 0;
			i.frag_bytes = 0;
			i.offset = 0;
			i.sparse = 0;
			break;
		}	
		case SQUASHFS_SYMLINK_TYPE: {
			squashfs_symlink_inode_header_1 *inodep =
				&header.symlink;

			if(swap) {
				squashfs_symlink_inode_header_1 sinodep;
				memcpy(&sinodep, block_ptr, sizeof(sinodep));
				SQUASHFS_SWAP_SYMLINK_INODE_HEADER_1(inodep,
					&sinodep);
			} else
				memcpy(inodep, block_ptr, sizeof(*inodep));

			i.symlink = malloc(inodep->symlink_size + 1);
			if(i.symlink == NULL)
				EXIT_UNSQUASH("read_inode: failed to malloc "
					"symlink data\n");
			strncpy(i.symlink, block_ptr +
				sizeof(squashfs_symlink_inode_header_1),
				inodep->symlink_size);
			i.symlink[inodep->symlink_size] = '\0';
			i.data = inodep->symlink_size;
			break;
		}
 		case SQUASHFS_BLKDEV_TYPE:
	 	case SQUASHFS_CHRDEV_TYPE: {
			squashfs_dev_inode_header_1 *inodep = &header.dev;

			if(swap) {
				squashfs_dev_inode_header_1 sinodep;
				memcpy(&sinodep, block_ptr, sizeof(sinodep));
				SQUASHFS_SWAP_DEV_INODE_HEADER_1(inodep,
					&sinodep);
			} else
				memcpy(inodep, block_ptr, sizeof(*inodep));

			i.data = inodep->rdev;
			break;
			}
		case SQUASHFS_FIFO_TYPE:
		case SQUASHFS_SOCKET_TYPE: {
			i.data = 0;
			break;
			}
		default:
			EXIT_UNSQUASH("Unknown inode type %d in "
				" read_inode_header_1!\n",
				header.base.inode_type);
	}
	return &i;
}