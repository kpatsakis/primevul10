Array HHVM_FUNCTION(gd_info) {
  Array ret = Array::CreateDArray();

  ret.set(s_GD_Version, PHP_GD_VERSION_STRING);

#ifdef ENABLE_GD_TTF
  ret.set(s_FreeType_Support, true);
#if HAVE_LIBFREETYPE
  ret.set(s_FreeType_Linkage, s_with_freetype);
#elif HAVE_LIBTTF
  ret.set(s_FreeType_Linkage, s_with_TTF_library);
#else
  ret.set(s_FreeType_Linkage, s_with_unknown_library);
#endif
#else
  ret.set(s_FreeType_Support, false);
#endif

#ifdef HAVE_LIBT1
  ret.set(s_T1Lib_Support, true);
#else
  ret.set(s_T1Lib_Support, false);
#endif
  ret.set(s_GIF_Read_Support, true);
  ret.set(s_GIF_Create_Support, true);
#ifdef HAVE_GD_JPG
  ret.set(s_JPG_Support, true);
#else
  ret.set(s_JPG_Support, false);
#endif
#ifdef HAVE_GD_PNG
  ret.set(s_PNG_Support, true);
#else
  ret.set(s_PNG_Support, false);
#endif
  ret.set(s_WBMP_Support, true);
#if defined(HAVE_GD_XPM) && defined(HAVE_GD_BUNDLED)
  ret.set(s_XPM_Support, true);
#else
  ret.set(s_XPM_Support, false);
#endif
  ret.set(s_XBM_Support, true);
#if defined(USE_GD_JISX0208) && defined(HAVE_GD_BUNDLED)
  ret.set(s_JIS_mapped_Japanese_Font_Support, true);
#else
  ret.set(s_JIS_mapped_Japanese_Font_Support, false);
#endif
  return ret;
}