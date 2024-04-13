void Item_string::print(String *str, enum_query_type query_type)
{
  const bool print_introducer=
    !(query_type & QT_WITHOUT_INTRODUCERS) && is_cs_specified();
  if (print_introducer)
  {
    str->append('_');
    str->append(collation.collation->csname);
  }

  str->append('\'');

  if (query_type & QT_TO_SYSTEM_CHARSET)
  {
    if (print_introducer)
    {
      /*
        Because we wrote an introducer, we must print str_value in its
        charset, and the resulting bytes must not be changed until they
        reach the end client.
        But the caller is asking for system_charset_info, and may later
        convert into character_set_results. That means two conversions: we
        must ensure that they don't change our printed bytes.
        So we print str_value in the least common denominator of the three
        charsets involved: ASCII. Non-ASCII characters are printed as \xFF
        sequences (which is ASCII too). This way, our bytes will not be
        changed.
      */
      ErrConvString tmp(str_value.ptr(), str_value.length(), &my_charset_bin);
      str->append(tmp.ptr());
    }
    else
    {
      if (my_charset_same(str_value.charset(), system_charset_info))
        str_value.print(str); // already in system_charset_info
      else // need to convert
      {
        THD *thd= current_thd;
        LEX_STRING utf8_lex_str;

        thd->convert_string(&utf8_lex_str,
                            system_charset_info,
                            str_value.c_ptr_safe(),
                            str_value.length(),
                            str_value.charset());

        String utf8_str(utf8_lex_str.str,
                        utf8_lex_str.length,
                        system_charset_info);

        utf8_str.print(str);
      }
    }
  }
  else
  {
    // Caller wants a result in the charset of str_value.
    str_value.print(str);
  }

  str->append('\'');
}