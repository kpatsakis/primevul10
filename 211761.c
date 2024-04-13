Variant HHVM_FUNCTION(imageaffine,
                      const Resource& image,
                      const Array& affine /* = Array() */,
                      const Array& clip /* = Array() */) {
  gdImagePtr src = get_valid_image_resource(image);
  if (!src) return false;
  gdImagePtr dst = nullptr;
  gdRect rect;
  gdRectPtr pRect = nullptr;
  int nelem = affine.size();
  int i;
  double daffine[6];

  if (nelem != 6) {
    raise_warning("imageaffine(): Affine array must have six elements");
    return false;
  }

  for (i = 0; i < nelem; i++) {
    if (affine[i].isInteger()) {
      daffine[i] = affine[i].toInt64();
    } else if (affine[i].isDouble() || affine[i].isString()) {
      daffine[i] = affine[i].toDouble();
    } else {
      raise_warning("imageaffine(): Invalid type for element %i", i);
      return false;
    }
  }

  if (!clip.empty()) {
    if (clip.exists(s_x)) {
      rect.x = clip[s_x].toInt64();
    } else {
      raise_warning("imageaffine(): Missing x position");
      return false;
    }
    if (clip.exists(s_y)) {
      rect.y = clip[s_y].toInt64();
    } else {
      raise_warning("imageaffine(): Missing y position");
      return false;
    } if (clip.exists(s_width)) {
      rect.width = clip[s_width].toInt64();
    } else {
      raise_warning("imageaffine(): Missing width position");
      return false;
    }
    if (clip.exists(s_height)) {
      rect.height = clip[s_height].toInt64();
    } else {
      raise_warning("imageaffine(): Missing height position");
      return false;
    }
    pRect = &rect;
  } else {
    rect.x = -1;
    rect.y = -1;
    rect.width = gdImageSX(src);
    rect.height = gdImageSY(src);
    pRect = nullptr;
  }

  if (gdTransformAffineGetImage(&dst, src, pRect, daffine) != GD_TRUE) {
    return false;
  }
  return Variant(req::make<Image>(dst));
}