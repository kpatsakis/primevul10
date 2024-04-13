static MagickBooleanType WriteFLIFImage(const ImageInfo *image_info,
  Image *image, ExceptionInfo *exception)
{
  FLIF_ENCODER
    *flifenc;

  FLIF_IMAGE
    *flifimage;

  int
    flif_status;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  register const PixelPacket
    *magick_restrict p;

  register ssize_t
    x;

  register unsigned char
    *magick_restrict qc;

  register unsigned short
    *magick_restrict qs;

  size_t
    columns,
    imageListLength,
    length,
    rows;

  ssize_t
    y;

  void
    *buffer;

  void
    *pixels;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns > 0xFFFF) || (image->rows > 0xFFFF))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  flifenc=flif_create_encoder();
  if (image_info->quality != UndefinedCompressionQuality)
    flif_encoder_set_lossy(flifenc,3*(100-image_info->quality));

  /* relatively fast encoding */
  flif_encoder_set_learn_repeat(flifenc,1);
  flif_encoder_set_split_threshold(flifenc,5461*8*5);

  columns=image->columns;
  rows=image->rows;

  /* Convert image to FLIFIMAGE */
  if (image->depth > 8)
    {
      flifimage=flif_create_image_HDR(image->columns,image->rows);
      length=sizeof(unsigned short)*4*image->columns;
    }
  else
    {
      flifimage=flif_create_image(image->columns,image->rows);
      length=sizeof(unsigned char)*4*image->columns;
    }
  if (flifimage == (FLIF_IMAGE *) NULL)
    {
      flif_destroy_encoder(flifenc);
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    }
  pixels=AcquireMagickMemory(length);
  if (pixels == (void *) NULL)
    {
      flif_destroy_image(flifimage);
      flif_destroy_encoder(flifenc);
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    }
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=GetVirtualPixels(image,0,y,image->columns,1,exception);
      if (p == (PixelPacket *) NULL)
        break;

      if (image->depth > 8)
        {
          qs=(unsigned short *) pixels;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            *qs++=ScaleQuantumToShort(GetPixelRed(p));
            *qs++=ScaleQuantumToShort(GetPixelGreen(p));
            *qs++=ScaleQuantumToShort(GetPixelBlue(p));
            if (image->matte != MagickFalse)
              *qs++=ScaleQuantumToShort(GetPixelAlpha(p));
            else
              *qs++=0xFFFF;
            p++;
          }
          flif_image_write_row_RGBA16(flifimage,y,pixels,length);
        }
      else
        {
          qc=pixels;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            *qc++=ScaleQuantumToChar(GetPixelRed(p));
            *qc++=ScaleQuantumToChar(GetPixelGreen(p));
            *qc++=ScaleQuantumToChar(GetPixelBlue(p));
            if (image->matte != MagickFalse)
              *qc++=ScaleQuantumToChar(GetPixelAlpha(p));
            else
              *qc++=0xFF;
            p++;
          }
          flif_image_write_row_RGBA8(flifimage,y,pixels,length);
        }
    }
    flif_image_set_frame_delay(flifimage,image->delay*100/
      image->ticks_per_second);
    flif_encoder_add_image(flifenc,flifimage);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    if ((columns != image->columns) || (rows != image->rows))
      {
        flif_destroy_image(flifimage);
        flif_destroy_encoder(flifenc);
        pixels=RelinquishMagickMemory(pixels);
        ThrowWriterException(ImageError,"FramesNotSameDimensions");
      }
    scene++;
    status=SetImageProgress(image,SaveImagesTag,scene,imageListLength);
    if (status == MagickFalse)
       break;
  } while (image_info->adjoin != MagickFalse);
  flif_destroy_image(flifimage);
  pixels=RelinquishMagickMemory(pixels);
  flif_status=flif_encoder_encode_memory(flifenc,&buffer,&length);
  if (flif_status)
    WriteBlob(image,length,buffer);
  CloseBlob(image);
  flif_destroy_encoder(flifenc);
  buffer=RelinquishMagickMemory(buffer);
  return(flif_status == 0 ? MagickFalse : MagickTrue);
}