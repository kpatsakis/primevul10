test_recode_command (xd3_stream *stream, int ignore)
{
  /* Things to test:
   * - with and without a source file (recode does not change)
   *
   * (recode may or may not change -- 8 variations)
   * - with and without adler32
   * - with and without app header
   * - with and without secondary
   */
  int has_source;
  int variant;
  int change;
  int ret;

  for (has_source = 0; has_source < 2; has_source++)
    {
      for (variant = 0; variant < 8; variant++)
	{
	  for (change = 0; change < 8; change++)
	    {
	      if ((ret = test_recode_command2 (stream, has_source,
					       variant, change)))
		{
		  return ret;
		}
	    }
	  DOT ();
	}
    }

  return 0;
}