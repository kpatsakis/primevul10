read_data_sectors_generic (void *p_user_data, void *p_buf, lsn_t i_lsn,
                           uint16_t i_blocksize, uint32_t i_blocks)
{
  int rc;
  if (0 > cdio_generic_lseek(p_user_data, i_blocksize*i_lsn, SEEK_SET))
    return DRIVER_OP_ERROR;
  rc = cdio_generic_read(p_user_data, p_buf, i_blocksize*i_blocks);
  if (rc > 0) return DRIVER_OP_SUCCESS;
  return DRIVER_OP_ERROR;
}