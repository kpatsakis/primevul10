iodev_os_open_file(gx_io_device * iodev, const char *fname, uint len,
                   const char *file_access, stream ** ps, gs_memory_t * mem)
{
    return file_open_stream(fname, len, file_access,
                            file_default_buffer_size, ps,
                            iodev, iodev->procs.gp_fopen, mem);
}