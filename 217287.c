MagickExport Image *StatisticImage(const Image *image,const StatisticType type,
  const size_t width,const size_t height,ExceptionInfo *exception)
{
  Image
    *statistic_image;

  statistic_image=StatisticImageChannel(image,DefaultChannels,type,width,
    height,exception);
  return(statistic_image);
}