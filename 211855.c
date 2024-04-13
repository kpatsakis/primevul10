exif_process_user_comment(image_info_type* /*ImageInfo*/, char** pszInfoPtr,
                          char** pszEncoding, char* szValuePtr, int ByteCount) {
  int   a;
  *pszEncoding = nullptr;
  /* Copy the comment */
  if (ByteCount>=8) {
    if (!memcmp(szValuePtr, "UNICODE\0", 8)) {
      PHP_STRDUP(*pszEncoding, (const char*)szValuePtr);
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
      return exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
    } else
    if (!memcmp(szValuePtr, "ASCII\0\0\0", 8)) {
      PHP_STRDUP(*pszEncoding, (const char*)szValuePtr);
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
    } else
    if (!memcmp(szValuePtr, "JIS\0\0\0\0\0", 8)) {
      /* JIS should be tanslated to MB or we leave it to the user */
      PHP_STRDUP(*pszEncoding, (const char*)szValuePtr);
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
      return exif_process_string_raw(pszInfoPtr, szValuePtr, ByteCount);
    } else
    if (!memcmp(szValuePtr, "\0\0\0\0\0\0\0\0", 8)) {
      /* 8 NULL means undefined and should be ASCII... */
      PHP_STRDUP(*pszEncoding, "UNDEFINED");
      szValuePtr = szValuePtr+8;
      ByteCount -= 8;
    }
  }

  /* Olympus has this padded with trailing spaces.  Remove these first. */
  if (ByteCount>0) {
    for (a=ByteCount-1;a && szValuePtr[a]==' ';a--) {
      (szValuePtr)[a] = '\0';
    }
  }

  /* normal text without encoding */
  exif_process_string(pszInfoPtr, szValuePtr, ByteCount);
  return strlen(*pszInfoPtr);
}