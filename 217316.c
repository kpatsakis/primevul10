MagickExport MagickBooleanType GetImageChannelMean(const Image *image,
  const ChannelType channel,double *mean,double *standard_deviation,
  ExceptionInfo *exception)
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
  channel_statistics[CompositeChannels].mean=0.0;
  channel_statistics[CompositeChannels].standard_deviation=0.0;
  if ((channel & RedChannel) != 0)
    {
      channel_statistics[CompositeChannels].mean+=
        channel_statistics[RedChannel].mean;
      channel_statistics[CompositeChannels].standard_deviation+=
        channel_statistics[RedChannel].standard_deviation;
      channels++;
    }
  if ((channel & GreenChannel) != 0)
    {
      channel_statistics[CompositeChannels].mean+=
        channel_statistics[GreenChannel].mean;
      channel_statistics[CompositeChannels].standard_deviation+=
        channel_statistics[GreenChannel].standard_deviation;
      channels++;
    }
  if ((channel & BlueChannel) != 0)
    {
      channel_statistics[CompositeChannels].mean+=
        channel_statistics[BlueChannel].mean;
      channel_statistics[CompositeChannels].standard_deviation+=
        channel_statistics[BlueChannel].standard_deviation;
      channels++;
    }
  if (((channel & OpacityChannel) != 0) && (image->matte != MagickFalse))
    {
      channel_statistics[CompositeChannels].mean+=
        (QuantumRange-channel_statistics[OpacityChannel].mean);
      channel_statistics[CompositeChannels].standard_deviation+=
        channel_statistics[OpacityChannel].standard_deviation;
      channels++;
    }
  if (((channel & IndexChannel) != 0) && (image->colorspace == CMYKColorspace))
    {
      channel_statistics[CompositeChannels].mean+=
        channel_statistics[BlackChannel].mean;
      channel_statistics[CompositeChannels].standard_deviation+=
        channel_statistics[CompositeChannels].standard_deviation;
      channels++;
    }
  channel_statistics[CompositeChannels].mean/=channels;
  channel_statistics[CompositeChannels].standard_deviation/=channels;
  *mean=channel_statistics[CompositeChannels].mean;
  *standard_deviation=channel_statistics[CompositeChannels].standard_deviation;
  channel_statistics=(ChannelStatistics *) RelinquishMagickMemory(
    channel_statistics);
  return(MagickTrue);
}