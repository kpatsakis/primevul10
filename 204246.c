parseZZZ (const char *data,
          size_t pos, size_t len,
	  EXTRACTOR_MetaDataProcessor proc,
	  void *proc_cls)
{
  size_t slen;
  size_t end;
  unsigned int i;

  end = pos + len;
  slen = strlen ("ps:SDict begin [");
  if ( (len <= slen) ||
       (0 != strncmp ("ps:SDict begin [ ", &data[pos], slen)) )
    return 0;
  pos += slen;
  while (pos < end)
    {
      for (i = 0; NULL != tmap[i].text; i++)
        {
          slen = strlen (tmap[i].text);
          if ( (pos + slen > end) ||
	       (0 != strncmp (&data[pos], tmap[i].text, slen)) )
	    continue;
	  pos += slen;
	  slen = pos;
	  while ((slen < end) && (data[slen] != ')'))
	    slen++;
	  slen = slen - pos;
	  {
	    char value[slen + 1];
	  
	    value[slen] = '\0';
	    memcpy (value, &data[pos], slen);
	    if (0 != proc (proc_cls, 
			   "dvi",
			   tmap[i].type,
			   EXTRACTOR_METAFORMAT_C_STRING,
			   "text/plain",
			   value,
			   slen + 1))
	      return 1;
	  }
	  pos += slen + 1;
	  break;
	}
      pos++;
    }
  return 0;
}