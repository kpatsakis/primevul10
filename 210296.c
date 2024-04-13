int main (int argc, char** argv)
{
   OSCTXT      jsonCtxt, cborCtxt;
   OSOCTET*    pMsgBuf = 0;
   size_t      msglen;
   OSBOOL      verbose = FALSE;
   const char* filename = "message.cbor";
   const char* outfname = "message.json";
   int         ret;

   /* Process command line arguments */
   if (argc > 1) {
      int i;
      for (i = 1; i < argc; i++) {
         if (!strcmp (argv[i], "-v")) verbose = TRUE;
         else if (!strcmp (argv[i], "-i")) filename = argv[++i];
         else if (!strcmp (argv[i], "-o")) outfname = argv[++i];
         else {
            printf ("usage: cbor2json [-v] [-i <filename>] [-o filename]\n");
            printf ("   -v  verbose mode: print trace info\n");
            printf ("   -i <filename>  read CBOR msg from <filename>\n");
            printf ("   -o <filename>  write JSON data to <filename>\n");
            return 1;
         }
      }
   }

   /* Initialize context structures */
   ret = rtxInitContext (&jsonCtxt);
   if (ret != 0) {
      rtxErrPrint (&jsonCtxt);
      return ret;
   }
   rtxErrInit();
   /* rtxSetDiag (&jsonCtxt, verbose); */

   ret = rtxInitContext (&cborCtxt);
   if (ret != 0) {
      rtxErrPrint (&cborCtxt);
      return ret;
   }
   /* rtxSetDiag (&cborCtxt, verbose); */

   /* Create file input stream */
#if 0
   /* Streaming not supported in open source version
   ret = rtxStreamFileCreateReader (&jsonCtxt, filename);
   */
#else
   /* Read input file into memory buffer */
   ret = rtxFileReadBinary (&cborCtxt, filename, &pMsgBuf, &msglen);
   if (0 == ret) {
      ret = rtxInitContextBuffer (&cborCtxt, pMsgBuf, msglen);
   }
#endif
   if (0 != ret) {
      rtxErrPrint (&jsonCtxt);
      rtxFreeContext (&jsonCtxt);
      rtxFreeContext (&cborCtxt);
      return ret;
   }

   /* Init JSON output buffer */
   ret = rtxInitContextBuffer (&jsonCtxt, 0, 0);
   if (0 != ret) {
      rtxErrPrint (&jsonCtxt);
      rtxFreeContext (&jsonCtxt);
      rtxFreeContext (&cborCtxt);
      return ret;
   }

   /* Invoke the translation function */
   ret = cbor2json (&cborCtxt, &jsonCtxt);

   if (0 == ret && cborCtxt.level != 0) 
      ret = LOG_RTERR (&cborCtxt, RTERR_UNBAL);

   if (0 == ret && 0 != outfname) {
      /* Write encoded JSON data to output file */
      OSRTSAFEPUTCHAR (&jsonCtxt, '\0');  /* null terminate buffer */
      int fileret = rtxFileWriteText 
         (outfname, (const char*)jsonCtxt.buffer.data);

      if (0 != fileret) {
         printf ("unable to write message data to '%s', status = %d\n", 
                 outfname, fileret);
      }
   }

   if (0 != ret) {
      rtxErrPrint (&jsonCtxt);
      rtxErrPrint (&cborCtxt);
   }

   rtxFreeContext (&jsonCtxt);
   rtxFreeContext (&cborCtxt);

   return ret;
}