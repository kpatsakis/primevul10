xd3_selftest (void)
{
#define DO_TEST(fn,flags,arg)                                         \
  do {                                                                \
    xd3_stream stream;                                                \
    xd3_config config;                                                \
    xd3_init_config (& config, flags);                                \
    XPR(NT "testing " #fn "%s...",                          \
             flags ? (" (" #flags ")") : "");                         \
    if ((ret = xd3_config_stream (& stream, & config) == 0) &&        \
        (ret = test_ ## fn (& stream, arg)) == 0) {                   \
      XPR(NTR " success\n");                                          \
    } else {                                                          \
      XPR(NTR " failed: %s: %s\n", xd3_errstring (& stream),          \
               xd3_mainerror (ret)); }                                \
    xd3_free_stream (& stream);                                       \
    if (ret != 0) { goto failure; }                                   \
  } while (0)

  int ret;
  DO_TEST (random_numbers, 0, 0);

  DO_TEST (decode_integer_end_of_input, 0, 0);
  DO_TEST (decode_integer_overflow, 0, 0);
  DO_TEST (encode_decode_uint32_t, 0, 0);
  DO_TEST (encode_decode_uint64_t, 0, 0);
  DO_TEST (usize_t_overflow, 0, 0);
  DO_TEST (forward_match, 0, 0);

  DO_TEST (address_cache, 0, 0);
  IF_GENCODETBL (DO_TEST (address_cache, XD3_ALT_CODE_TABLE, 0));

  DO_TEST (string_matching, 0, 0);
  DO_TEST (choose_instruction, 0, 0);
  DO_TEST (identical_behavior, 0, 0);
  DO_TEST (in_memory, 0, 0);

  IF_GENCODETBL (DO_TEST (choose_instruction, XD3_ALT_CODE_TABLE, 0));
  IF_GENCODETBL (DO_TEST (encode_code_table, 0, 0));

  DO_TEST (iopt_flush_instructions, 0, 0);
  DO_TEST (source_cksum_offset, 0, 0);

  DO_TEST (decompress_single_bit_error, 0, 3);
  DO_TEST (decompress_single_bit_error, XD3_ADLER32, 3);

  IF_LZMA (DO_TEST (decompress_single_bit_error, XD3_SEC_LZMA, 54));
  IF_FGK (DO_TEST (decompress_single_bit_error, XD3_SEC_FGK, 3));
  IF_DJW (DO_TEST (decompress_single_bit_error, XD3_SEC_DJW, 8));

  /* There are many expected non-failures for ALT_CODE_TABLE because
   * not all of the instruction codes are used. */
  IF_GENCODETBL (
	 DO_TEST (decompress_single_bit_error, XD3_ALT_CODE_TABLE, 224));

#if SHELL_TESTS
  DO_TEST (force_behavior, 0, 0);
  DO_TEST (stdout_behavior, 0, 0);
  DO_TEST (no_output, 0, 0);
  DO_TEST (appheader, 0, 0);
  DO_TEST (command_line_arguments, 0, 0);

#if EXTERNAL_COMPRESSION
  DO_TEST (source_decompression, 0, 0);
  DO_TEST (externally_compressed_io, 0, 0);
#endif

  DO_TEST (recode_command, 0, 0);
#endif

  IF_LZMA (DO_TEST (secondary_lzma, 0, 1));
  IF_DJW (DO_TEST (secondary_huff, 0, DJW_MAX_GROUPS));
  IF_FGK (DO_TEST (secondary_fgk, 0, 1));

  DO_TEST (compressed_stream_overflow, 0, 0);
  IF_LZMA (DO_TEST (compressed_stream_overflow, XD3_SEC_LZMA, 0));

failure:
  test_cleanup ();
  return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
#undef DO_TEST
}