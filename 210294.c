static int cborTagNotSupp (OSCTXT* pctxt, OSOCTET tag)
{
   char numbuf[10];
   char errtext[80];

   rtxUIntToCharStr (tag, numbuf, sizeof(numbuf), 0);
   rtxStrJoin (errtext, sizeof(errtext), "CBOR tag ", numbuf, 0, 0, 0);
   rtxErrAddStrParm (pctxt, errtext);

   return RTERR_NOTSUPP;
}