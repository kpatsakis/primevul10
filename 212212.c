bool HHVM_FUNCTION(imagefilledpolygon, const Resource& image,
    const Array& points, int64_t num_points, int64_t color) {
  return php_imagepolygon(image, points, num_points, color, 1);
}