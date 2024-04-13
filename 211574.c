Variant HHVM_FUNCTION(imagecrop, const Resource& image, const Array& rect) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImagePtr imcropped = nullptr;
  gdRect gdrect;
  if (rect.exists(s_x)) {
    gdrect.x = rect[s_x].toInt64();
  } else {
    raise_warning("imagecrop(): Missing x position");
    return false;
  }
  if (rect.exists(s_y)) {
    gdrect.y = rect[s_y].toInt64();
  } else {
    raise_warning("imagecrop(): Missing y position");
    return false;
  }
  if (rect.exists(s_width)) {
    gdrect.width = rect[s_width].toInt64();
  } else {
    raise_warning("imagecrop(): Missing width position");
    return false;
  }
  if (rect.exists(s_height)) {
    gdrect.height = rect[s_height].toInt64();
  } else {
    raise_warning("imagecrop(): Missing height position");
    return false;
  }

  imcropped = gdImageCrop(im, &gdrect);

  if (!imcropped) {
    return false;
  }
  return Variant(req::make<Image>(imcropped));
}