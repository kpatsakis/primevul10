main_help (void)
{
  main_version();

  /* Note: update wiki when command-line features change */
  XPR(NTR "usage: xdelta3 [command/options] [input [output]]\n");
  XPR(NTR "make patch:\n");
  XPR(NTR "\n");
  XPR(NTR "  xdelta3.exe -e -s old_file new_file delta_file\n");
  XPR(NTR "\n");
  XPR(NTR "apply patch:\n");
  XPR(NTR "\n");
  XPR(NTR "  xdelta3.exe -d -s old_file delta_file decoded_new_file\n");
  XPR(NTR "\n");
  XPR(NTR "special command names:\n");
  XPR(NTR "    config      prints xdelta3 configuration\n");
  XPR(NTR "    decode      decompress the input\n");
  XPR(NTR "    encode      compress the input%s\n",
     XD3_ENCODER ? "" : " [Not compiled]");
#if REGRESSION_TEST
  XPR(NTR "    test        run the builtin tests\n");
#endif
#if VCDIFF_TOOLS
  XPR(NTR "special commands for VCDIFF inputs:\n");
  XPR(NTR "    printdelta  print information about the entire delta\n");
  XPR(NTR "    printhdr    print information about the first window\n");
  XPR(NTR "    printhdrs   print information about all windows\n");
  XPR(NTR "    recode      encode with new application/secondary settings\n");
  XPR(NTR "    merge       merge VCDIFF inputs (see below)\n");
#endif
  XPR(NTR "merge patches:\n");
  XPR(NTR "\n");
  XPR(NTR "  xdelta3 merge -m 1.vcdiff -m 2.vcdiff 3.vcdiff merged.vcdiff\n");
  XPR(NTR "\n");
  XPR(NTR "standard options:\n");
  XPR(NTR "   -0 .. -9     compression level\n");
  XPR(NTR "   -c           use stdout\n");
  XPR(NTR "   -d           decompress\n");
  XPR(NTR "   -e           compress%s\n",
     XD3_ENCODER ? "" : " [Not compiled]");
  XPR(NTR "   -f           force (overwrite, ignore trailing garbage)\n");
#if EXTERNAL_COMPRESSION
  XPR(NTR "   -F           force the external-compression subprocess\n");
#endif
  XPR(NTR "   -h           show help\n");
  XPR(NTR "   -q           be quiet\n");
  XPR(NTR "   -v           be verbose (max 2)\n");
  XPR(NTR "   -V           show version\n");

  XPR(NTR "memory options:\n");
  XPR(NTR "   -B bytes     source window size\n");
  XPR(NTR "   -W bytes     input window size\n");
  XPR(NTR "   -P size      compression duplicates window\n");
  XPR(NTR "   -I size      instruction buffer size (0 = unlimited)\n");

  XPR(NTR "compression options:\n");
  XPR(NTR "   -s source    source file to copy from (if any)\n");
  XPR(NTR "   -S [djw|fgk] enable/disable secondary compression\n");
  XPR(NTR "   -N           disable small string-matching compression\n");
  XPR(NTR "   -D           disable external decompression (encode/decode)\n");
  XPR(NTR "   -R           disable external recompression (decode)\n");
  XPR(NTR "   -n           disable checksum (encode/decode)\n");
  XPR(NTR "   -C           soft config (encode, undocumented)\n");
  XPR(NTR "   -A [apphead] disable/provide application header (encode)\n");
  XPR(NTR "   -J           disable output (check/compute only)\n");
  XPR(NTR "   -T           use alternate code table (test)\n");
  XPR(NTR "   -m           arguments for \"merge\"\n");

  XPR(NTR "the XDELTA environment variable may contain extra args:\n");
  XPR(NTR "   XDELTA=\"-s source-x.y.tar.gz\" \\\n");
  XPR(NTR "   tar --use-compress-program=xdelta3 \\\n");
  XPR(NTR "       -cf target-x.z.tar.gz.vcdiff target-x.y\n");
  return EXIT_FAILURE;
}