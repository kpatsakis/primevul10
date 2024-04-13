static int cborElemNameToJson (OSCTXT* pCborCtxt, OSCTXT* pJsonCtxt)
{
   char* pElemName = 0;
   OSOCTET ub;
   int ret;

   /* Read byte from stream */
   ret = rtxReadBytes (pCborCtxt, &ub, 1);
   if (0 != ret) return LOG_RTERR (pCborCtxt, ret);

   /* Decode element name (note: only string type is currently supported) */
   ret = rtCborDecDynUTF8Str (pCborCtxt, ub, &pElemName);
   if (0 != ret) return LOG_RTERR (pCborCtxt, ret);

   /* Encode map element name as string */
   ret = rtJsonEncStringValue (pJsonCtxt, (const OSUTF8CHAR*)pElemName);
   rtxMemFreePtr (pCborCtxt, pElemName);
   if (0 != ret) return LOG_RTERR (pJsonCtxt, ret);

   OSRTSAFEPUTCHAR (pJsonCtxt, ':');

   return 0;
}