bool HHVM_FUNCTION(imageconvolution, const Resource& image,
    const Array& matrix, double div, double offset) {
  gdImagePtr im_src = cast<Image>(image)->get();
  if (!im_src) return false;
  int nelem = matrix.size();
  int i, j;
  float mtx[3][3] = {{0,0,0}, {0,0,0}, {0,0,0}};
  Variant v;
  Array row;

  if (nelem != 3) {
    raise_warning("You must have 3x3 array");
    return false;
  }
  for (i=0; i<3; i++) {
    if (matrix.exists(i) && (v = matrix[i]).isArray()) {
      if ((row = v.toArray()).size() != 3) {
        raise_warning("You must have 3x3 array");
        return false;
      }

      for (j=0; j<3; j++) {
        if (row.exists(j)) {
          mtx[i][j] = row[j].toDouble();
        } else {
          raise_warning("You must have a 3x3 matrix");
          return false;
        }
      }
    }
  }
  if (hphp_gdImageConvolution(im_src, mtx, div, offset)) {
    return true;
  } else {
    return false;
  }
}