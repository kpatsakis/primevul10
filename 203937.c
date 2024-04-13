usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
	     program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [STRINGS]...\n\
"), program_name);
      fputs (_("\
Internationalized Domain Name (IDN) convert STRINGS, or standard input.\n\
\n\
"), stdout);
      fputs (_("\
Command line interface to the internationalized domain name library.\n\
\n\
All strings are expected to be encoded in the preferred charset used\n\
by your locale.  Use `--debug' to find out what this charset is.  You\n\
can override the charset used by setting environment variable CHARSET.\n\
\n\
To process a string that starts with `-', for example `-foo', use `--'\n\
to signal the end of parameters, as in `idn --quiet -a -- -foo'.\n\
\n\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -h, --help               Print help and exit\n\
  -V, --version            Print version and exit\n\
"), stdout);
      fputs (_("\
  -s, --stringprep         Prepare string according to nameprep profile\n\
  -d, --punycode-decode    Decode Punycode\n\
  -e, --punycode-encode    Encode Punycode\n\
  -a, --idna-to-ascii      Convert to ACE according to IDNA (default mode)\n\
  -u, --idna-to-unicode    Convert from ACE according to IDNA\n\
"), stdout);
      fputs (_("\
      --allow-unassigned   Toggle IDNA AllowUnassigned flag (default off)\n\
      --usestd3asciirules  Toggle IDNA UseSTD3ASCIIRules flag (default off)\n\
"), stdout);
      fputs (_("\
      --no-tld             Don't check string for TLD specific rules\n\
                             Only for --idna-to-ascii and --idna-to-unicode\n\
"), stdout);
      fputs (_("\
  -n, --nfkc               Normalize string according to Unicode v3.2 NFKC\n\
"), stdout);
      fputs (_("\
  -p, --profile=STRING     Use specified stringprep profile instead\n\
                             Valid stringprep profiles: `Nameprep',\n\
                             `iSCSI', `Nodeprep', `Resourceprep', \n\
                             `trace', `SASLprep'\n\
"), stdout);
      fputs (_("\
      --debug              Print debugging information\n\
      --quiet              Silent operation\n\
"), stdout);
      emit_bug_reporting_address ();
    }
  exit (status);
}