reset_defaults(void)
{
  option_stdout = 0;
  option_force = 0;
  option_verbose = DEFAULT_VERBOSE;
  option_quiet = 0;
  option_appheader = NULL;
  option_use_secondary = 0;
  option_secondary = NULL;
  option_use_altcodetable = 0;
  option_smatch_config = NULL;
  option_no_compress = 0;
  option_no_output = 0;
  option_source_filename = NULL;
  program_name = NULL;
  appheader_used = NULL;
  main_bdata = NULL;
  main_bsize = 0;
  allow_fake_source = 0;
  option_smatch_config = NULL;

  main_lru_reset();

  option_use_appheader = 1;
  option_use_checksum = 1;
#if EXTERNAL_COMPRESSION
  option_force2 = 0;
  option_decompress_inputs  = 1;
  option_recompress_outputs = 1;
  num_subprocs = 0;
#endif
#if VCDIFF_TOOLS
  option_print_cpymode = 1;
#endif
  option_level = XD3_DEFAULT_LEVEL;
  option_iopt_size = XD3_DEFAULT_IOPT_SIZE;
  option_winsize = XD3_DEFAULT_WINSIZE;
  option_srcwinsz = XD3_DEFAULT_SRCWINSZ;
  option_sprevsz = XD3_DEFAULT_SPREVSZ;
}