main_read_primary_input (main_file   *file,
			 uint8_t     *buf,
			 size_t       size,
			 size_t      *nread)
{
#if EXTERNAL_COMPRESSION
  if (option_decompress_inputs && file->flags & RD_FIRST)
    {
      file->flags &= ~RD_FIRST;
      return main_secondary_decompress_check (file, buf, size, nread);
    }
#endif

  return main_file_read (file, buf, size, nread, "input read failed");
}