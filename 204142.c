cdio_generic_read_form1_sector (void * user_data, void *data, lsn_t lsn)
{
  if (0 > cdio_generic_lseek(user_data, CDIO_CD_FRAMESIZE*lsn, SEEK_SET))
    return DRIVER_OP_ERROR;
  return cdio_generic_read(user_data, data, CDIO_CD_FRAMESIZE);
}