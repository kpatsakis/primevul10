static void SetHeaderFromIPL(Image *image, IPLInfo *ipl){
  image->columns = ipl->width;
  image->rows = ipl->height;
  image->depth = ipl->depth;
  image->x_resolution = 1;
  image->y_resolution = 1;
}