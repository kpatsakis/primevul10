test_usize_t_overflow (xd3_stream *stream, int unused)
{
  if (USIZE_T_OVERFLOW (USIZE_T_MAX, 0)) { goto fail; }
  if (USIZE_T_OVERFLOW (0, USIZE_T_MAX)) { goto fail; }
  if (USIZE_T_OVERFLOW (USIZE_T_MAX / 2, USIZE_T_MAX / 2)) { goto fail; }
  if (USIZE_T_OVERFLOW (USIZE_T_MAX / 2, USIZE_T_MAX / 2 + 1)) { goto fail; }

  if (! USIZE_T_OVERFLOW (USIZE_T_MAX, 1)) { goto fail; }
  if (! USIZE_T_OVERFLOW (1, USIZE_T_MAX)) { goto fail; }
  if (! USIZE_T_OVERFLOW (USIZE_T_MAX / 2 + 1, USIZE_T_MAX / 2 + 1)) { goto fail; }

  return 0;

 fail:
  stream->msg = "incorrect overflow computation";
  return XD3_INTERNAL;
}