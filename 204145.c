set_track_flags(track_flags_t *p_track_flag, uint8_t i_flag)
{
  p_track_flag->preemphasis = ( i_flag & CDIO_TRACK_FLAG_PRE_EMPHASIS )
    ? CDIO_TRACK_FLAG_TRUE : CDIO_TRACK_FLAG_FALSE;

  p_track_flag->copy_permit = ( i_flag & CDIO_TRACK_FLAG_COPY_PERMITTED )
    ? CDIO_TRACK_FLAG_TRUE : CDIO_TRACK_FLAG_FALSE;

  p_track_flag->channels = ( i_flag & CDIO_TRACK_FLAG_FOUR_CHANNEL_AUDIO )
    ? 4 : 2;
}