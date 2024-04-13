MagickExport MagickBooleanType DrawImage(Image *image,const DrawInfo *draw_info)
{
  return(RenderMVGContent(image,draw_info,0));
}