get_discmode_cd_generic (void *p_user_data )
{
  generic_img_private_t *p_env = p_user_data;
  track_t i_track;
  discmode_t discmode=CDIO_DISC_MODE_NO_INFO;

  if (!p_env->toc_init)
    p_env->cdio->op.read_toc (p_user_data);

  if (!p_env->toc_init)
    return CDIO_DISC_MODE_NO_INFO;

  for (i_track = p_env->i_first_track;
       i_track < p_env->i_first_track + p_env->i_tracks ;
       i_track ++) {
    track_format_t track_fmt =
      p_env->cdio->op.get_track_format(p_env, i_track);

    switch(track_fmt) {
    case TRACK_FORMAT_AUDIO:
      switch(discmode) {
        case CDIO_DISC_MODE_NO_INFO:
          discmode = CDIO_DISC_MODE_CD_DA;
          break;
        case CDIO_DISC_MODE_CD_DA:
        case CDIO_DISC_MODE_CD_MIXED:
        case CDIO_DISC_MODE_ERROR:
          /* No change*/
          break;
      default:
          discmode = CDIO_DISC_MODE_CD_MIXED;
      }
      break;
    case TRACK_FORMAT_XA:
      switch(discmode) {
        case CDIO_DISC_MODE_NO_INFO:
          discmode = CDIO_DISC_MODE_CD_XA;
          break;
        case CDIO_DISC_MODE_CD_XA:
        case CDIO_DISC_MODE_CD_MIXED:
        case CDIO_DISC_MODE_ERROR:
          /* No change*/
          break;
      default:
        discmode = CDIO_DISC_MODE_CD_MIXED;
      }
      break;
    case TRACK_FORMAT_CDI:
    case TRACK_FORMAT_DATA:
      switch(discmode) {
        case CDIO_DISC_MODE_NO_INFO:
          discmode = CDIO_DISC_MODE_CD_DATA;
          break;
        case CDIO_DISC_MODE_CD_DATA:
        case CDIO_DISC_MODE_CD_MIXED:
        case CDIO_DISC_MODE_ERROR:
          /* No change*/
          break;
      default:
        discmode = CDIO_DISC_MODE_CD_MIXED;
      }
      break;
    case TRACK_FORMAT_ERROR:
    default:
      discmode = CDIO_DISC_MODE_ERROR;
    }
  }
  return discmode;
}