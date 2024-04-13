get_discmode_generic (void *p_user_data )
{
  generic_img_private_t *p_env = p_user_data;

  /* See if this is a DVD. */
  cdio_dvd_struct_t dvd;  /* DVD READ STRUCT for layer 0. */

  memset(&dvd, 0, sizeof(dvd));

  dvd.physical.type = CDIO_DVD_STRUCT_PHYSICAL;
  dvd.physical.layer_num = 0;
  if (0 == mmc_get_dvd_struct_physical (p_env->cdio, &dvd)) {
    switch(dvd.physical.layer[0].book_type) {
    case CDIO_DVD_BOOK_DVD_ROM:    return CDIO_DISC_MODE_DVD_ROM;
    case CDIO_DVD_BOOK_DVD_RAM:    return CDIO_DISC_MODE_DVD_RAM;
    case CDIO_DVD_BOOK_DVD_R:      return CDIO_DISC_MODE_DVD_R;
    case CDIO_DVD_BOOK_DVD_RW:     return CDIO_DISC_MODE_DVD_RW;
    case CDIO_DVD_BOOK_HD_DVD_ROM: return CDIO_DISC_MODE_HD_DVD_ROM;
    case CDIO_DVD_BOOK_HD_DVD_RAM: return CDIO_DISC_MODE_HD_DVD_RAM;
    case CDIO_DVD_BOOK_HD_DVD_R:   return CDIO_DISC_MODE_HD_DVD_R;
    case CDIO_DVD_BOOK_DVD_PR:     return CDIO_DISC_MODE_DVD_PR;
    case CDIO_DVD_BOOK_DVD_PRW:    return CDIO_DISC_MODE_DVD_PRW;
    case CDIO_DVD_BOOK_DVD_PRW_DL: return CDIO_DISC_MODE_DVD_PRW_DL;
    case CDIO_DVD_BOOK_DVD_PR_DL:  return CDIO_DISC_MODE_DVD_PR_DL;
    default: return CDIO_DISC_MODE_DVD_OTHER;
    }
  }

  return get_discmode_cd_generic(p_user_data);
}