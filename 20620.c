static MagickBooleanType WriteRAWImage(const ImageInfo *image_info,Image *image)
{
  MagickOffsetType
    scene;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  MagickBooleanType
    status;

  register const PixelPacket
    *p;

  size_t
    imageListLength,
    length;

  ssize_t
    count,
    y;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  switch (*image->magick)
  {
    case 'A':
    case 'a':
    {
      quantum_type=AlphaQuantum;
      break;
    }
    case 'B':
    case 'b':
    {
      quantum_type=BlueQuantum;
      break;
    }
    case 'C':
    case 'c':
    {
      quantum_type=CyanQuantum;
      if (image->colorspace == CMYKColorspace)
        break;
      ThrowWriterException(ImageError,"ColorSeparatedImageRequired");
    }
    case 'g':
    case 'G':
    {
      quantum_type=GreenQuantum;
      break;
    }
    case 'I':
    case 'i':
    {
      quantum_type=IndexQuantum;
      break;
    }
    case 'K':
    case 'k':
    {
      quantum_type=BlackQuantum;
      if (image->colorspace == CMYKColorspace)
        break;
      ThrowWriterException(ImageError,"ColorSeparatedImageRequired");
    }
    case 'M':
    case 'm':
    {
      quantum_type=MagentaQuantum;
      if (image->colorspace == CMYKColorspace)
        break;
      ThrowWriterException(ImageError,"ColorSeparatedImageRequired");
    }
    case 'o':
    case 'O':
    {
      quantum_type=OpacityQuantum;
      break;
    }
    case 'R':
    case 'r':
    {
      quantum_type=RedQuantum;
      break;
    }
    case 'Y':
    case 'y':
    {
      quantum_type=YellowQuantum;
      if (image->colorspace == CMYKColorspace)
        break;
      ThrowWriterException(ImageError,"ColorSeparatedImageRequired");
    }
    default:
    {
      quantum_type=GrayQuantum;
      break;
    }
  }
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    /*
      Convert image to RAW raster pixels.
    */
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    pixels=GetQuantumPixels(quantum_info);
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      length=ExportQuantumPixels(image,(const CacheView *) NULL,quantum_info,
        quantum_type,pixels,&image->exception);
      count=WriteBlob(image,length,pixels);
      if (count != (ssize_t) length)
        break;
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
            image->rows);
          if (status == MagickFalse)
            break;
        }
    }
    quantum_info=DestroyQuantumInfo(quantum_info);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(MagickTrue);
}