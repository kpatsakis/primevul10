  void moduleInit() override {
    HHVM_FE(gd_info);
    HHVM_FE(getimagesize);
    HHVM_FE(getimagesizefromstring);
    HHVM_FE(image_type_to_extension);
    HHVM_FE(image_type_to_mime_type);
    HHVM_FE(image2wbmp);
    HHVM_FE(imageaffine);
    HHVM_FE(imageaffinematrixconcat);
    HHVM_FE(imageaffinematrixget);
    HHVM_FE(imagealphablending);
    HHVM_FE(imageantialias);
    HHVM_FE(imagearc);
    HHVM_FE(imagechar);
    HHVM_FE(imagecharup);
    HHVM_FE(imagecolorallocate);
    HHVM_FE(imagecolorallocatealpha);
    HHVM_FE(imagecolorat);
    HHVM_FE(imagecolorclosest);
    HHVM_FE(imagecolorclosestalpha);
    HHVM_FE(imagecolorclosesthwb);
    HHVM_FE(imagecolordeallocate);
    HHVM_FE(imagecolorexact);
    HHVM_FE(imagecolorexactalpha);
    HHVM_FE(imagecolormatch);
    HHVM_FE(imagecolorresolve);
    HHVM_FE(imagecolorresolvealpha);
    HHVM_FE(imagecolorset);
    HHVM_FE(imagecolorsforindex);
    HHVM_FE(imagecolorstotal);
    HHVM_FE(imagecolortransparent);
    HHVM_FE(imageconvolution);
    HHVM_FE(imagecopy);
    HHVM_FE(imagecopymerge);
    HHVM_FE(imagecopymergegray);
    HHVM_FE(imagecopyresampled);
    HHVM_FE(imagecopyresized);
    HHVM_FE(imagecreate);
    HHVM_FE(imagecreatefromgd2part);
    HHVM_FE(imagecreatefromgd);
    HHVM_FE(imagecreatefromgd2);
    HHVM_FE(imagecreatefromgif);
#ifdef HAVE_GD_JPG
    HHVM_FE(imagecreatefromjpeg);
#endif
#ifdef HAVE_GD_PNG
    HHVM_FE(imagecreatefrompng);
#endif
#ifdef HAVE_LIBVPX
    HHVM_FE(imagecreatefromwebp);
#endif
    HHVM_FE(imagecreatefromstring);
    HHVM_FE(imagecreatefromwbmp);
    HHVM_FE(imagecreatefromxbm);
#if defined(HAVE_GD_XPM) && defined(HAVE_GD_BUNDLED)
    HHVM_FE(imagecreatefromxpm);
#endif
    HHVM_FE(imagecreatetruecolor);
    HHVM_FE(imagecrop);
    HHVM_FE(imagecropauto);
    HHVM_FE(imagedashedline);
    HHVM_FE(imagedestroy);
    HHVM_FE(imageellipse);
    HHVM_FE(imagefill);
    HHVM_FE(imagefilledarc);
    HHVM_FE(imagefilledellipse);
    HHVM_FE(imagefilledpolygon);
    HHVM_FE(imagefilledrectangle);
    HHVM_FE(imagefilltoborder);
    HHVM_FE(imagefilledellipse);
    HHVM_FE(imagefilledpolygon);
    HHVM_FE(imagefilledrectangle);
    HHVM_FE(imagefilltoborder);
    HHVM_FE(imagefilter);
    HHVM_FE(imageflip);
    HHVM_FE(imagefontheight);
    HHVM_FE(imagefontwidth);
#if defined(ENABLE_GD_TTF) && HAVE_LIBFREETYPE
    HHVM_FE(imageftbbox);
    HHVM_FE(imagefttext);
#endif
    HHVM_FE(imagegammacorrect);
    HHVM_FE(imagegd2);
    HHVM_FE(imagegd);
    HHVM_FE(imagegif);
    HHVM_FE(imageinterlace);
    HHVM_FE(imageistruecolor);
#ifdef HAVE_GD_JPG
    HHVM_FE(imagejpeg);
#endif
    HHVM_FE(imagelayereffect);
    HHVM_FE(imageline);
    HHVM_FE(imageloadfont);
#ifdef HAVE_GD_PNG
    HHVM_FE(imagepng);
#endif
#ifdef HAVE_LIBVPX
    HHVM_FE(imagewebp);
#endif
    HHVM_FE(imagepolygon);
    HHVM_FE(imagerectangle);
    HHVM_FE(imagerotate);
    HHVM_FE(imagesavealpha);
    HHVM_FE(imagescale);
    HHVM_FE(imagesetbrush);
    HHVM_FE(imagesetinterpolation);
    HHVM_FE(imagesetpixel);
    HHVM_FE(imagesetstyle);
    HHVM_FE(imagesetthickness);
    HHVM_FE(imagesettile);
    HHVM_FE(imagestring);
    HHVM_FE(imagestringup);
    HHVM_FE(imagesx);
    HHVM_FE(imagesy);
    HHVM_FE(imagetruecolortopalette);
#ifdef ENABLE_GD_TTF
    HHVM_FE(imagettfbbox);
    HHVM_FE(imagettftext);
#endif
    HHVM_FE(imagetypes);
    HHVM_FE(imagewbmp);

    HHVM_FE(iptcembed);
    HHVM_FE(iptcparse);

    HHVM_FE(jpeg2wbmp);
    HHVM_FE(png2wbmp);

    HHVM_FE(imagepalettecopy);

    HHVM_RC_INT(IMG_GIF, IMAGE_TYPE_GIF);
    HHVM_RC_INT(IMG_JPG, IMAGE_TYPE_JPEG);
    HHVM_RC_INT(IMG_JPEG, IMAGE_TYPE_JPEG);
    HHVM_RC_INT(IMG_PNG, IMAGE_TYPE_PNG);
    HHVM_RC_INT(IMG_WBMP, IMAGE_TYPE_WBMP);
    HHVM_RC_INT(IMG_XPM, IMAGE_TYPE_XPM);

    /* special colours for gd */
    HHVM_RC_INT(IMG_COLOR_TILED, gdTiled);
    HHVM_RC_INT(IMG_COLOR_STYLED, gdStyled);
    HHVM_RC_INT(IMG_COLOR_BRUSHED, gdBrushed);
    HHVM_RC_INT(IMG_COLOR_STYLEDBRUSHED, gdStyledBrushed);
    HHVM_RC_INT(IMG_COLOR_TRANSPARENT, gdTransparent);

    /* for imagefilledarc */
    HHVM_RC_INT(IMG_ARC_ROUNDED, gdArc);
    HHVM_RC_INT(IMG_ARC_PIE, gdPie);
    HHVM_RC_INT(IMG_ARC_CHORD, gdChord);
    HHVM_RC_INT(IMG_ARC_NOFILL, gdNoFill);
    HHVM_RC_INT(IMG_ARC_EDGED, gdEdged);

    /* GD2 image format types */
    HHVM_RC_INT(IMG_GD2_RAW, GD2_FMT_RAW);
    HHVM_RC_INT(IMG_GD2_COMPRESSED, GD2_FMT_COMPRESSED);
    HHVM_RC_INT(IMG_FLIP_HORIZONTAL, GD_FLIP_HORINZONTAL);
    HHVM_RC_INT(IMG_FLIP_VERTICAL, GD_FLIP_VERTICAL);
    HHVM_RC_INT(IMG_FLIP_BOTH, GD_FLIP_BOTH);
    HHVM_RC_INT(IMG_EFFECT_REPLACE, gdEffectReplace);
    HHVM_RC_INT(IMG_EFFECT_ALPHABLEND, gdEffectAlphaBlend);
    HHVM_RC_INT(IMG_EFFECT_NORMAL, gdEffectNormal);
    HHVM_RC_INT(IMG_EFFECT_OVERLAY, gdEffectOverlay);

    HHVM_RC_INT(IMG_CROP_DEFAULT, GD_CROP_DEFAULT);
    HHVM_RC_INT(IMG_CROP_TRANSPARENT, GD_CROP_TRANSPARENT);
    HHVM_RC_INT(IMG_CROP_BLACK, GD_CROP_BLACK);
    HHVM_RC_INT(IMG_CROP_WHITE, GD_CROP_WHITE);
    HHVM_RC_INT(IMG_CROP_SIDES, GD_CROP_SIDES);
    HHVM_RC_INT(IMG_CROP_THRESHOLD, GD_CROP_THRESHOLD);

    HHVM_RC_INT(IMG_BELL, GD_BELL);
    HHVM_RC_INT(IMG_BESSEL, GD_BESSEL);
    HHVM_RC_INT(IMG_BILINEAR_FIXED, GD_BILINEAR_FIXED);
    HHVM_RC_INT(IMG_BICUBIC, GD_BICUBIC);
    HHVM_RC_INT(IMG_BICUBIC_FIXED, GD_BICUBIC_FIXED);
    HHVM_RC_INT(IMG_BLACKMAN, GD_BLACKMAN);
    HHVM_RC_INT(IMG_BOX, GD_BOX);
    HHVM_RC_INT(IMG_BSPLINE, GD_BSPLINE);
    HHVM_RC_INT(IMG_CATMULLROM, GD_CATMULLROM);
    HHVM_RC_INT(IMG_GAUSSIAN, GD_GAUSSIAN);
    HHVM_RC_INT(IMG_GENERALIZED_CUBIC, GD_GENERALIZED_CUBIC);
    HHVM_RC_INT(IMG_HERMITE, GD_HERMITE);
    HHVM_RC_INT(IMG_HAMMING, GD_HAMMING);
    HHVM_RC_INT(IMG_HANNING, GD_HANNING);
    HHVM_RC_INT(IMG_MITCHELL, GD_MITCHELL);
    HHVM_RC_INT(IMG_POWER, GD_POWER);
    HHVM_RC_INT(IMG_QUADRATIC, GD_QUADRATIC);
    HHVM_RC_INT(IMG_SINC, GD_SINC);
    HHVM_RC_INT(IMG_NEAREST_NEIGHBOUR, GD_NEAREST_NEIGHBOUR);
    HHVM_RC_INT(IMG_WEIGHTED4, GD_WEIGHTED4);
    HHVM_RC_INT(IMG_TRIANGLE, GD_TRIANGLE);

    HHVM_RC_INT(IMG_AFFINE_TRANSLATE, GD_AFFINE_TRANSLATE);
    HHVM_RC_INT(IMG_AFFINE_SCALE, GD_AFFINE_SCALE);
    HHVM_RC_INT(IMG_AFFINE_ROTATE, GD_AFFINE_ROTATE);
    HHVM_RC_INT(IMG_AFFINE_SHEAR_HORIZONTAL, GD_AFFINE_SHEAR_HORIZONTAL);
    HHVM_RC_INT(IMG_AFFINE_SHEAR_VERTICAL, GD_AFFINE_SHEAR_VERTICAL);

    HHVM_RC_INT(IMG_FILTER_BRIGHTNESS, IMAGE_FILTER_BRIGHTNESS);
    HHVM_RC_INT(IMG_FILTER_COLORIZE, IMAGE_FILTER_COLORIZE);
    HHVM_RC_INT(IMG_FILTER_CONTRAST, IMAGE_FILTER_CONTRAST);
    HHVM_RC_INT(IMG_FILTER_EDGEDETECT, IMAGE_FILTER_EDGEDETECT);
    HHVM_RC_INT(IMG_FILTER_EMBOSS, IMAGE_FILTER_EMBOSS);
    HHVM_RC_INT(IMG_FILTER_GAUSSIAN_BLUR, IMAGE_FILTER_GAUSSIAN_BLUR);
    HHVM_RC_INT(IMG_FILTER_GRAYSCALE, IMAGE_FILTER_GRAYSCALE);
    HHVM_RC_INT(IMG_FILTER_MEAN_REMOVAL, IMAGE_FILTER_MEAN_REMOVAL);
    HHVM_RC_INT(IMG_FILTER_NEGATE, IMAGE_FILTER_NEGATE);
    HHVM_RC_INT(IMG_FILTER_SELECTIVE_BLUR, IMAGE_FILTER_SELECTIVE_BLUR);
    HHVM_RC_INT(IMG_FILTER_SMOOTH, IMAGE_FILTER_SMOOTH);
    HHVM_RC_INT(IMG_FILTER_PIXELATE, IMAGE_FILTER_PIXELATE);

    HHVM_RC_INT(IMAGETYPE_GIF, IMAGE_FILETYPE_GIF);
    HHVM_RC_INT(IMAGETYPE_JPEG, IMAGE_FILETYPE_JPEG);
    HHVM_RC_INT(IMAGETYPE_PNG, IMAGE_FILETYPE_PNG);
    HHVM_RC_INT(IMAGETYPE_SWF, IMAGE_FILETYPE_SWF);
    HHVM_RC_INT(IMAGETYPE_PSD, IMAGE_FILETYPE_PSD);
    HHVM_RC_INT(IMAGETYPE_BMP, IMAGE_FILETYPE_BMP);
    HHVM_RC_INT(IMAGETYPE_TIFF_II, IMAGE_FILETYPE_TIFF_II);
    HHVM_RC_INT(IMAGETYPE_TIFF_MM, IMAGE_FILETYPE_TIFF_MM);
    HHVM_RC_INT(IMAGETYPE_JPC, IMAGE_FILETYPE_JPC);
    HHVM_RC_INT(IMAGETYPE_JP2, IMAGE_FILETYPE_JP2);
    HHVM_RC_INT(IMAGETYPE_JPX, IMAGE_FILETYPE_JPX);
    HHVM_RC_INT(IMAGETYPE_JB2, IMAGE_FILETYPE_JB2);
    HHVM_RC_INT(IMAGETYPE_IFF, IMAGE_FILETYPE_IFF);
    HHVM_RC_INT(IMAGETYPE_WBMP, IMAGE_FILETYPE_WBMP);
    HHVM_RC_INT(IMAGETYPE_XBM, IMAGE_FILETYPE_XBM);
    HHVM_RC_INT(IMAGETYPE_ICO, IMAGE_FILETYPE_ICO);
    HHVM_RC_INT(IMAGETYPE_UNKNOWN, IMAGE_FILETYPE_UNKNOWN);
    HHVM_RC_INT(IMAGETYPE_COUNT, IMAGE_FILETYPE_COUNT);
    HHVM_RC_INT(IMAGETYPE_SWC, IMAGE_FILETYPE_SWC);
    HHVM_RC_INT(IMAGETYPE_JPEG2000, IMAGE_FILETYPE_JPC);

#ifdef GD_VERSION_STRING
    HHVM_RC_STR(GD_VERSION, GD_VERSION_STRING);
#endif

#if defined(GD_MAJOR_VERSION) && defined(GD_MINOR_VERSION) && \
    defined(GD_RELEASE_VERSION) && defined(GD_EXTRA_VERSION)
    HHVM_RC_INT_SAME(GD_MAJOR_VERSION);
    HHVM_RC_INT_SAME(GD_MINOR_VERSION);
    HHVM_RC_INT_SAME(GD_RELEASE_VERSION);
    HHVM_RC_STR_SAME(GD_EXTRA_VERSION);
#endif

#ifdef HAVE_GD_PNG
    HHVM_RC_INT(PNG_NO_FILTER, 0x00);
    HHVM_RC_INT(PNG_FILTER_NONE, 0x08);
    HHVM_RC_INT(PNG_FILTER_SUB, 0x10);
    HHVM_RC_INT(PNG_FILTER_UP, 0x20);
    HHVM_RC_INT(PNG_FILTER_AVG, 0x40);
    HHVM_RC_INT(PNG_FILTER_PAETH, 0x80);
    HHVM_RC_INT(PNG_ALL_FILTERS, 0x08 | 0x10 | 0x20 | 0x40 | 0x80);
#endif

    HHVM_RC_BOOL(GD_BUNDLED, true);

    loadSystemlib();
  }