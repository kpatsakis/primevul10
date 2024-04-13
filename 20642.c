static MagickBooleanType DecodeImage(Image *image,const ssize_t opacity)
{
  ExceptionInfo
    *exception;

  IndexPacket
    index;

  int
    c;

  LZWInfo
    *lzw_info;

  ssize_t
    offset,
    y;

  unsigned char
    data_size;

  size_t
    pass;

  /*
    Allocate decoder tables.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  data_size=(unsigned char) ReadBlobByte(image);
  if (data_size > MaximumLZWBits)
    ThrowBinaryException(CorruptImageError,"CorruptImage",image->filename);
  lzw_info=AcquireLZWInfo(image,data_size);
  if (lzw_info == (LZWInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  exception=(&image->exception);
  pass=0;
  offset=0;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register IndexPacket
      *magick_restrict indexes;

    register ssize_t
      x;

    register PixelPacket
      *magick_restrict q;

    q=QueueAuthenticPixels(image,0,offset,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetAuthenticIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; )
    {
      c=ReadBlobLZWByte(lzw_info);
      if (c < 0)
        break;
      index=ConstrainColormapIndex(image,(ssize_t) c);
      SetPixelIndex(indexes+x,index);
      SetPixelRGBO(q,image->colormap+(ssize_t) index);
      SetPixelOpacity(q,(ssize_t) index == opacity ? TransparentOpacity :
        OpaqueOpacity);
      x++;
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    if (x < (ssize_t) image->columns)
      break;
    if (image->interlace == NoInterlace)
      offset++;
    else
      {
        switch (pass)
        {
          case 0:
          default:
          {
            offset+=8;
            break;
          }
          case 1:
          {
            offset+=8;
            break;
          }
          case 2:
          {
            offset+=4;
            break;
          }
          case 3:
          {
            offset+=2;
            break;
          }
        }
      if ((pass == 0) && (offset >= (ssize_t) image->rows))
        {
          pass++;
          offset=4;
        }
      if ((pass == 1) && (offset >= (ssize_t) image->rows))
        {
          pass++;
          offset=2;
        }
      if ((pass == 2) && (offset >= (ssize_t) image->rows))
        {
          pass++;
          offset=1;
        }
    }
  }
  lzw_info=RelinquishLZWInfo(lzw_info);
  if (y < (ssize_t) image->rows)
    ThrowBinaryException(CorruptImageError,"CorruptImage",image->filename);
  return(MagickTrue);
}