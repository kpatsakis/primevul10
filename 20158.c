static void lite_font_map( wmfAPI* API, wmfFont* font)
{
  wmfFontData
    *font_data;

  wmf_magick_font_t
    *magick_font;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    *exception;

  const TypeInfo
    *type_info,
    *type_info_base;

  const char
    *wmf_font_name;

  if (font == 0)
    return;

  font_data = (wmfFontData*)API->font_data;
  font->user_data = font_data->user_data;
  magick_font = (wmf_magick_font_t*)font->user_data;
  wmf_font_name = WMF_FONT_NAME(font);

  if (magick_font->ps_name != (char *) NULL)
    magick_font->ps_name=DestroyString(magick_font->ps_name);

  exception=ddata->exception;
  type_info_base=GetTypeInfo("*",exception);
  if (type_info_base == 0)
     return;

  /* Certain short-hand font names are not the proper Windows names
     and should be promoted to the proper names */
  if (LocaleCompare(wmf_font_name,"Times") == 0)
    wmf_font_name = "Times New Roman";
  else if (LocaleCompare(wmf_font_name,"Courier") == 0)
    wmf_font_name = "Courier New";

  /* Look for a family-based best-match */
  if (!magick_font->ps_name)
    {
      int
        target_weight;

      if (WMF_FONT_WEIGHT(font) == 0)
        target_weight = 400;
      else
        target_weight = WMF_FONT_WEIGHT(font);
      type_info=GetTypeInfoByFamily(wmf_font_name,AnyStyle,AnyStretch,
        target_weight,exception);
      if (type_info == (const TypeInfo *) NULL)
        type_info=GetTypeInfoByFamily(wmf_font_name,AnyStyle,AnyStretch,0,
          exception);
      if (type_info != (const TypeInfo *) NULL)
        CloneString(&magick_font->ps_name,type_info->name);
    }

  /* Now let's try simple substitution mappings from WMFFontMap */
  if (!magick_font->ps_name)
    {
      char
        target[MagickPathExtent];

      int
        target_weight = 400,
        want_italic = MagickFalse,
        want_bold = MagickFalse,
        i;

      if ( WMF_FONT_WEIGHT(font) != 0 )
        target_weight = WMF_FONT_WEIGHT(font);

      if ( (target_weight > 550) || ((strstr(wmf_font_name,"Bold") ||
                                     strstr(wmf_font_name,"Heavy") ||
                                     strstr(wmf_font_name,"Black"))) )
        want_bold = MagickTrue;

      if ( (WMF_FONT_ITALIC(font)) || ((strstr(wmf_font_name,"Italic") ||
                                       strstr(wmf_font_name,"Oblique"))) )
        want_italic = MagickTrue;

      (void) CopyMagickString(target,"Times",MagickPathExtent);
      for( i=0; SubFontMap[i].name != NULL; i++ )
        {
          if (LocaleCompare(wmf_font_name, SubFontMap[i].name) == 0)
            {
              (void) CopyMagickString(target,SubFontMap[i].mapping,
                MagickPathExtent);
              break;
            }
        }

      for( i=0; WMFFontMap[i].name != NULL; i++ )
        {
          if (LocaleNCompare(WMFFontMap[i].name,target,strlen(WMFFontMap[i].name)) == 0)
            {
              if (want_bold && want_italic)
                CloneString(&magick_font->ps_name,WMFFontMap[i].bolditalic);
              else if (want_italic)
                CloneString(&magick_font->ps_name,WMFFontMap[i].italic);
              else if (want_bold)
                CloneString(&magick_font->ps_name,WMFFontMap[i].bold);
              else
                CloneString(&magick_font->ps_name,WMFFontMap[i].normal);
            }
        }
    }

#if 0
  printf("\nlite_font_map\n");
  printf("WMF_FONT_NAME           = \"%s\"\n", WMF_FONT_NAME(font));
  printf("WMF_FONT_WEIGHT         = %i\n",  WMF_FONT_WEIGHT(font));
  printf("WMF_FONT_PSNAME         = \"%s\"\n", WMF_FONT_PSNAME(font));
  fflush(stdout);
#endif

}