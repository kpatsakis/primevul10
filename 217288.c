MagickExport MagickBooleanType GetImageChannelEntropy(const Image *image,
  const ChannelType channel,double *entropy,ExceptionInfo *exception)
{
  ChannelStatistics
    *channel_statistics;

  size_t
    channels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  channel_statistics=GetImageChannelStatistics(image,exception);
  if (channel_statistics == (ChannelStatistics *) NULL)
    return(MagickFalse);
  channels=0;
  channel_statistics[CompositeChannels].entropy=0.0;
  if ((channel & RedChannel) != 0)
    {
      channel_statistics[CompositeChannels].entropy+=
        channel_statistics[RedChannel].entropy;
      channels++;
    }
  if ((channel & GreenChannel) != 0)
    {
      channel_statistics[CompositeChannels].entropy+=
        channel_statistics[GreenChannel].entropy;
      channels++;
    }
  if ((channel & BlueChannel) != 0)
    {
      channel_statistics[CompositeChannels].entropy+=
        channel_statistics[BlueChannel].entropy;
      channels++;
    }
  if (((channel & OpacityChannel) != 0) && (image->matte != MagickFalse))
    {
      channel_statistics[CompositeChannels].entropy+=
        channel_statistics[OpacityChannel].entropy;
      channels++;
    }
  if (((channel & IndexChannel) != 0) &&
      (image->colorspace == CMYKColorspace))
    {
      channel_statistics[CompositeChannels].entropy+=
        channel_statistics[BlackChannel].entropy;
      channels++;
    }
  channel_statistics[CompositeChannels].entropy/=channels;
  *entropy=channel_statistics[CompositeChannels].entropy;
  channel_statistics=(ChannelStatistics *) RelinquishMagickMemory(
    channel_statistics);
  return(MagickTrue);
}