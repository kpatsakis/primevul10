static bool php_imagepolygon(const Resource& image,
                             const Array& points, int num_points,
                             int color, int filled) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdPointPtr pts;
  int nelem, i;

  nelem = points.size();
  if (nelem < 6) {
    raise_warning("You must have at least 3 points in your array");
    return false;
  }

  if (nelem < num_points * 2) {
    raise_warning("Trying to use %d points in array with only %d points",
                    num_points, nelem/2);
    return false;
  }

  pts = (gdPointPtr)IM_MALLOC(num_points * sizeof(gdPoint));
  CHECK_ALLOC_R(pts, (num_points * sizeof(gdPoint)), false);

  for (i = 0; i < num_points; i++) {
    if (points.exists(i * 2)) {
      pts[i].x = points[i * 2].toInt32();
    }
    if (points.exists(i * 2 + 1)) {
      pts[i].y = points[i * 2 + 1].toInt32();
    }
  }

  if (filled) {
    gdImageFilledPolygon(im, pts, num_points, color);
  } else {
    color = SetupAntiAliasedColor(im, color);
    gdImagePolygon(im, pts, num_points, color);
  }

  IM_FREE(pts);
  return true;
}