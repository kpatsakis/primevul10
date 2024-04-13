static struct inode *read_inode(unsigned int start_block, unsigned int offset)
{
	static union squashfs_inode_header header;
	long long start = sBlk.s.inode_table_start + start_block;
	long long bytes = lookup_entry(inode_table_hash, start);
	char *block_ptr = inode_table + bytes + offset;
	static struct inode i;

	TRACE("read_inode: reading inode [%d:%d]\n", start_block,  offset);

	if(bytes == -1)
		EXIT_UNSQUASH("read_inode: inode table block %lld not found\n",
			start); 		

	SQUASHFS_SWAP_BASE_INODE_HEADER(block_ptr, &header.base);

	i.uid = (uid_t) id_table[header.base.uid];
	i.gid = (uid_t) id_table[header.base.guid];
	i.mode = lookup_type[header.base.inode_type] | header.base.mode;
	i.type = header.base.inode_type;
	i.time = header.base.mtime;
	i.inode_number = header.base.inode_number;

	switch(header.base.inode_type) {
		case SQUASHFS_DIR_TYPE: {
			struct squashfs_dir_inode_header *inode = &header.dir;

			SQUASHFS_SWAP_DIR_INODE_HEADER(block_ptr, inode);

			i.data = inode->file_size;
			i.offset = inode->offset;
			i.start = inode->start_block;
			i.xattr = SQUASHFS_INVALID_XATTR;
			break;
		}
		case SQUASHFS_LDIR_TYPE: {
			struct squashfs_ldir_inode_header *inode = &header.ldir;

			SQUASHFS_SWAP_LDIR_INODE_HEADER(block_ptr, inode);

			i.data = inode->file_size;
			i.offset = inode->offset;
			i.start = inode->start_block;
			i.xattr = inode->xattr;
			break;
		}
		case SQUASHFS_FILE_TYPE: {
			struct squashfs_reg_inode_header *inode = &header.reg;

			SQUASHFS_SWAP_REG_INODE_HEADER(block_ptr, inode);

			i.data = inode->file_size;
			i.frag_bytes = inode->fragment == SQUASHFS_INVALID_FRAG
				?  0 : inode->file_size % sBlk.s.block_size;
			i.fragment = inode->fragment;
			i.offset = inode->offset;
			i.blocks = inode->fragment == SQUASHFS_INVALID_FRAG ?
				(i.data + sBlk.s.block_size - 1) >>
				sBlk.s.block_log :
				i.data >> sBlk.s.block_log;
			i.start = inode->start_block;
			i.sparse = 0;
			i.block_ptr = block_ptr + sizeof(*inode);
			i.xattr = SQUASHFS_INVALID_XATTR;
			break;
		}	
		case SQUASHFS_LREG_TYPE: {
			struct squashfs_lreg_inode_header *inode = &header.lreg;

			SQUASHFS_SWAP_LREG_INODE_HEADER(block_ptr, inode);

			i.data = inode->file_size;
			i.frag_bytes = inode->fragment == SQUASHFS_INVALID_FRAG
				?  0 : inode->file_size % sBlk.s.block_size;
			i.fragment = inode->fragment;
			i.offset = inode->offset;
			i.blocks = inode->fragment == SQUASHFS_INVALID_FRAG ?
				(inode->file_size + sBlk.s.block_size - 1) >>
				sBlk.s.block_log :
				inode->file_size >> sBlk.s.block_log;
			i.start = inode->start_block;
			i.sparse = inode->sparse != 0;
			i.block_ptr = block_ptr + sizeof(*inode);
			i.xattr = inode->xattr;
			break;
		}	
		case SQUASHFS_SYMLINK_TYPE:
		case SQUASHFS_LSYMLINK_TYPE: {
			struct squashfs_symlink_inode_header *inode = &header.symlink;

			SQUASHFS_SWAP_SYMLINK_INODE_HEADER(block_ptr, inode);

			i.symlink = malloc(inode->symlink_size + 1);
			if(i.symlink == NULL)
				EXIT_UNSQUASH("read_inode: failed to malloc "
					"symlink data\n");
			strncpy(i.symlink, block_ptr +
				sizeof(struct squashfs_symlink_inode_header),
				inode->symlink_size);
			i.symlink[inode->symlink_size] = '\0';
			i.data = inode->symlink_size;

			if(header.base.inode_type == SQUASHFS_LSYMLINK_TYPE)
				SQUASHFS_SWAP_INTS(block_ptr +
					sizeof(struct squashfs_symlink_inode_header) +
					inode->symlink_size, &i.xattr, 1);
			else
				i.xattr = SQUASHFS_INVALID_XATTR;
			break;
		}
 		case SQUASHFS_BLKDEV_TYPE:
	 	case SQUASHFS_CHRDEV_TYPE: {
			struct squashfs_dev_inode_header *inode = &header.dev;

			SQUASHFS_SWAP_DEV_INODE_HEADER(block_ptr, inode);

			i.data = inode->rdev;
			i.xattr = SQUASHFS_INVALID_XATTR;
			break;
		}
 		case SQUASHFS_LBLKDEV_TYPE:
	 	case SQUASHFS_LCHRDEV_TYPE: {
			struct squashfs_ldev_inode_header *inode = &header.ldev;

			SQUASHFS_SWAP_LDEV_INODE_HEADER(block_ptr, inode);

			i.data = inode->rdev;
			i.xattr = inode->xattr;
			break;
		}
		case SQUASHFS_FIFO_TYPE:
		case SQUASHFS_SOCKET_TYPE:
			i.data = 0;
			i.xattr = SQUASHFS_INVALID_XATTR;
			break;
		case SQUASHFS_LFIFO_TYPE:
		case SQUASHFS_LSOCKET_TYPE: {
			struct squashfs_lipc_inode_header *inode = &header.lipc;

			SQUASHFS_SWAP_LIPC_INODE_HEADER(block_ptr, inode);

			i.data = 0;
			i.xattr = inode->xattr;
			break;
		}
		default:
			EXIT_UNSQUASH("Unknown inode type %d in read_inode!\n",
				header.base.inode_type);
	}
	return &i;
}