init(void)
{
#ifdef USE_CALLOUT

    int id;
    OnigEncoding enc;
    char* name;
    unsigned int args[4];
    OnigValue    opts[4];

    enc = ONIG_ENCODING_UTF16_LE;

    name = "F\000A\000I\000L\000\000\000";            BC0_P(name, fail);
    name = "M\000I\000S\000M\000A\000T\000C\000H\000\000\000"; BC0_P(name, mismatch);

    name = "M\000A\000X\000\000\000";
    args[0] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    args[1] = ONIG_TYPE_CHAR;
    opts[0].c = 'X';
    BC_B_O(name, max, 2, args, 1, opts);

    name = "E\000R\000R\000O\000R\000\000\000";
    args[0] = ONIG_TYPE_LONG; opts[0].l = ONIG_ABORT;
    BC_P_O(name, error, 1, args, 1, opts);

    name = "C\000O\000U\000N\000T\000\000\000";
    args[0] = ONIG_TYPE_CHAR; opts[0].c = '>';
    BC_B_O(name, count, 1, args, 1, opts);

    name = "T\000O\000T\000A\000L\000_\000C\000O\000U\000N\000T\000\000\000";
    args[0] = ONIG_TYPE_CHAR; opts[0].c = '>';
    BC_B_O(name, total_count, 1, args, 1, opts);

    name = "C\000M\000P\000\000\000";
    args[0] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    args[1] = ONIG_TYPE_STRING;
    args[2] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    BC_P(name, cmp, 3, args);

#endif /* USE_CALLOUT */

  return ONIG_NORMAL;
}