  void moduleInit() override {
    setlocale(LC_CTYPE, "");

    HHVM_FE(addcslashes);
    HHVM_FE(stripcslashes);
    HHVM_FE(addslashes);
    HHVM_FE(stripslashes);
    HHVM_FE(bin2hex);
    HHVM_FE(hex2bin);
    HHVM_FE(nl2br);
    HHVM_FE(quotemeta);
    HHVM_FE(str_shuffle);
    HHVM_FE(strrev);
    HHVM_FE(strtolower);
    HHVM_FE(strtoupper);
    HHVM_FE(ucfirst);
    HHVM_FE(lcfirst);
    HHVM_FE(ucwords);
    HHVM_FE(strip_tags);
    HHVM_FE(trim);
    HHVM_FE(ltrim);
    HHVM_FE(rtrim);
    HHVM_FE(chop);
    HHVM_FE(explode);
    HHVM_FE(implode);
    HHVM_FALIAS(join, implode);
    HHVM_FE(str_split);
    HHVM_FE(chunk_split);
    HHVM_FE(strtok);
    HHVM_FE(str_replace);
    HHVM_FE(str_replace_with_count);
    HHVM_FE(str_ireplace);
    HHVM_FE(str_ireplace_with_count);
    HHVM_FE(substr_replace);
    HHVM_FE(substr);
    HHVM_FE(str_pad);
    HHVM_FE(str_repeat);
    HHVM_FE(html_entity_decode);
    HHVM_FE(htmlentities);
    HHVM_FE(htmlspecialchars_decode);
    HHVM_FE(htmlspecialchars);
    HHVM_FE(fb_htmlspecialchars);
    HHVM_FE(quoted_printable_encode);
    HHVM_FE(quoted_printable_decode);
    HHVM_FE(convert_uudecode);
    HHVM_FE(convert_uuencode);
    HHVM_FE(str_rot13);
    HHVM_FE(crc32);
    HHVM_FE(crypt);
    HHVM_FE(md5);
    HHVM_FE(sha1);
    HHVM_FE(strtr);
    HHVM_FE(convert_cyr_string);
    HHVM_FE(get_html_translation_table);
    HHVM_FE(hebrev);
    HHVM_FE(hebrevc);
    HHVM_FE(setlocale);
    HHVM_FE(localeconv);
    HHVM_FE(nl_langinfo);
    HHVM_FE(sscanf);
    HHVM_FE(chr);
    HHVM_FE(ord);
    HHVM_FE(money_format);
    HHVM_FE(number_format);
    HHVM_FE(strcmp);
    HHVM_FE(strncmp);
    HHVM_FE(strnatcmp);
    HHVM_FE(strcasecmp);
    HHVM_FE(strncasecmp);
    HHVM_FE(strnatcasecmp);
    HHVM_FE(strcoll);
    HHVM_FE(substr_compare);
    HHVM_FE(strchr);
    HHVM_FE(strrchr);
    HHVM_FE(strstr);
    HHVM_FE(stristr);
    HHVM_FE(strpbrk);
    HHVM_FE(strpos);
    HHVM_FE(stripos);
    HHVM_FE(strrpos);
    HHVM_FE(strripos);
    HHVM_FE(substr_count);
    HHVM_FE(strspn);
    HHVM_FE(strcspn);
    HHVM_FE(strlen);
    HHVM_FE(str_getcsv);
    HHVM_FE(count_chars);
    HHVM_FE(str_word_count);
    HHVM_FE(levenshtein);
    HHVM_FE(similar_text);
    HHVM_FE(soundex);
    HHVM_FE(metaphone);

    HHVM_RC_INT(ENT_COMPAT, k_ENT_HTML_QUOTE_DOUBLE);
    HHVM_RC_INT(ENT_NOQUOTES, k_ENT_HTML_QUOTE_NONE);
    HHVM_RC_INT(ENT_QUOTES, k_ENT_QUOTES);
    HHVM_RC_INT(ENT_IGNORE, k_ENT_HTML_IGNORE_ERRORS);
    HHVM_RC_INT(ENT_SUBSTITUTE, k_ENT_HTML_SUBSTITUTE_ERRORS);
    HHVM_RC_INT(ENT_HTML401, k_ENT_HTML_DOC_HTML401);
    HHVM_RC_INT(ENT_XML1, k_ENT_HTML_DOC_XML1);
    HHVM_RC_INT(ENT_XHTML, k_ENT_HTML_DOC_XHTML);
    HHVM_RC_INT(ENT_HTML5, k_ENT_HTML_DOC_HTML5);
    HHVM_RC_INT(ENT_FB_UTF8, k_ENT_FB_UTF8);
    HHVM_RC_INT(ENT_FB_UTF8_ONLY, k_ENT_FB_UTF8_ONLY);

    HHVM_RC_INT(HTML_SPECIALCHARS, k_HTML_SPECIALCHARS);
    HHVM_RC_INT(HTML_ENTITIES, k_HTML_ENTITIES);

    HHVM_RC_INT(STR_PAD_LEFT, k_STR_PAD_LEFT);
    HHVM_RC_INT(STR_PAD_RIGHT, k_STR_PAD_RIGHT);
    HHVM_RC_INT(STR_PAD_BOTH, k_STR_PAD_BOTH);

    HHVM_RC_INT_SAME(LC_CTYPE);
    HHVM_RC_INT_SAME(LC_NUMERIC);
    HHVM_RC_INT_SAME(LC_TIME);
    HHVM_RC_INT_SAME(LC_COLLATE);
    HHVM_RC_INT_SAME(LC_MONETARY);
    HHVM_RC_INT_SAME(LC_ALL);
#ifdef LC_MESSAGES
    HHVM_RC_INT_SAME(LC_MESSAGES);
#endif

#ifdef YESEXPR
    HHVM_RC_INT_SAME(YESEXPR);
#endif
#ifdef NOEXPR
    HHVM_RC_INT_SAME(NOEXPR);
#endif
    HHVM_RC_INT(CHAR_MAX, std::numeric_limits<char>::max());

    HHVM_RC_STR(HPHP_TRIM_CHARLIST, k_HPHP_TRIM_CHARLIST);

#ifdef ABDAY_1
    HHVM_RC_INT_SAME(ABDAY_1);
    HHVM_RC_INT_SAME(ABDAY_2);
    HHVM_RC_INT_SAME(ABDAY_3);
    HHVM_RC_INT_SAME(ABDAY_4);
    HHVM_RC_INT_SAME(ABDAY_5);
    HHVM_RC_INT_SAME(ABDAY_6);
    HHVM_RC_INT_SAME(ABDAY_7);
#endif
#ifdef DAY_1
    HHVM_RC_INT_SAME(DAY_1);
    HHVM_RC_INT_SAME(DAY_2);
    HHVM_RC_INT_SAME(DAY_3);
    HHVM_RC_INT_SAME(DAY_4);
    HHVM_RC_INT_SAME(DAY_5);
    HHVM_RC_INT_SAME(DAY_6);
    HHVM_RC_INT_SAME(DAY_7);
#endif
#ifdef ABMON_1
    HHVM_RC_INT_SAME(ABMON_1);
    HHVM_RC_INT_SAME(ABMON_2);
    HHVM_RC_INT_SAME(ABMON_3);
    HHVM_RC_INT_SAME(ABMON_4);
    HHVM_RC_INT_SAME(ABMON_5);
    HHVM_RC_INT_SAME(ABMON_6);
    HHVM_RC_INT_SAME(ABMON_7);
    HHVM_RC_INT_SAME(ABMON_8);
    HHVM_RC_INT_SAME(ABMON_9);
    HHVM_RC_INT_SAME(ABMON_10);
    HHVM_RC_INT_SAME(ABMON_11);
    HHVM_RC_INT_SAME(ABMON_12);
#endif
#ifdef MON_1
    HHVM_RC_INT_SAME(MON_1);
    HHVM_RC_INT_SAME(MON_2);
    HHVM_RC_INT_SAME(MON_3);
    HHVM_RC_INT_SAME(MON_4);
    HHVM_RC_INT_SAME(MON_5);
    HHVM_RC_INT_SAME(MON_6);
    HHVM_RC_INT_SAME(MON_7);
    HHVM_RC_INT_SAME(MON_8);
    HHVM_RC_INT_SAME(MON_9);
    HHVM_RC_INT_SAME(MON_10);
    HHVM_RC_INT_SAME(MON_11);
    HHVM_RC_INT_SAME(MON_12);
#endif

    // These are ostensibly bools,
    // but for historical reasons are expressed as ints
    HHVM_RC_INT(CRYPT_BLOWFISH, 1);
    HHVM_RC_INT(CRYPT_EXT_DES, 0);
    HHVM_RC_INT(CRYPT_MD5, 1);
    HHVM_RC_INT(CRYPT_STD_DES, 1);

    HHVM_RC_INT(CRYPT_SALT_LENGTH, 12);

    loadSystemlib();
  }