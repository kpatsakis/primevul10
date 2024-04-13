translate_mode(struct file *file)
{
	int file_mode = 0;
	if ((file->f_flags & O_ACCMODE) != O_RDONLY)
		file_mode |= SNDRV_SEQ_OSS_FILE_WRITE;
	if ((file->f_flags & O_ACCMODE) != O_WRONLY)
		file_mode |= SNDRV_SEQ_OSS_FILE_READ;
	if (file->f_flags & O_NONBLOCK)
		file_mode |= SNDRV_SEQ_OSS_FILE_NONBLOCK;
	return file_mode;
}