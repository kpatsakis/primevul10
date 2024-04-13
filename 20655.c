static Image *ReadRLEImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define SkipLinesOp  0x01
#define SetColorOp  0x02
#define SkipPixelsOp  0x03
#define ByteDataOp  0x05
#define RunDataOp  0x06
#define EOFOp  0x07
#define ThrowRLEException(exception,message) \
{ \
  if (colormap != (unsigned char *) NULL) \
    colormap=(unsigned char *) RelinquishMagickMemory(colormap); \
  if (pixel_info != (MemoryInfo *) NULL) \
    pixel_info=RelinquishVirtualMemory(pixel_info); \
  ThrowReaderException((exception),(message)); \
}


  char
    magick[12];

  Image
    *image;

  IndexPacket
    index;

  int
    opcode,
    operand,
    status;

  MagickStatusType
    flags;

  MagickSizeType
    number_pixels;

  MemoryInfo
    *pixel_info;

  register IndexPacket
    *indexes;

  register ssize_t
    x;

  register PixelPacket
    *q;

  register ssize_t
    i;

  register unsigned char
    *p;

  size_t
    bits_per_pixel,
    map_length,
    number_colormaps,
    number_planes,
    number_planes_filled,
    one,
    pixel_info_length;

  ssize_t
    count,
    offset,
    y;

  unsigned char
    background_color[256],
    *colormap,
    pixel,
    plane,
    *pixels;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  /*
    Determine if this a RLE file.
  */
  colormap=(unsigned char *) NULL;
  pixel_info=(MemoryInfo *) NULL;
  count=ReadBlob(image,2,(unsigned char *) magick);
  if ((count != 2) || (memcmp(magick,"\122\314",2) != 0))
    ThrowRLEException(CorruptImageError,"ImproperImageHeader");
  do
  {
    /*
      Read image header.
    */
    image->page.x=(ssize_t) ReadBlobLSBShort(image);
    image->page.y=(ssize_t) ReadBlobLSBShort(image);
    image->columns=ReadBlobLSBShort(image);
    image->rows=ReadBlobLSBShort(image);
    flags=(MagickStatusType) ReadBlobByte(image);
    image->matte=flags & 0x04 ? MagickTrue : MagickFalse;
    number_planes=(size_t) ReadBlobByte(image);
    bits_per_pixel=(size_t) ReadBlobByte(image);
    number_colormaps=(size_t) ReadBlobByte(image);
    map_length=(unsigned char) ReadBlobByte(image);
    if (map_length >= 22)
      ThrowRLEException(CorruptImageError,"ImproperImageHeader");
    if (EOFBlob(image) != MagickFalse)
      ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
    one=1;
    map_length=one << map_length;
    if ((number_planes == 0) || (number_planes == 2) || ((flags & 0x04) &&
        ((number_planes <= 2) || number_planes > 254)) || (bits_per_pixel != 8))
      ThrowRLEException(CorruptImageError,"ImproperImageHeader");
    if (number_planes > 4)
      ThrowRLEException(CorruptImageError,"ImproperImageHeader");
    if ((image->columns == 0) || (image->columns >= 32768) ||
        (image->rows == 0) || (image->rows >= 32768))
      ThrowRLEException(CorruptImageError,"ImproperImageHeader");
    if (flags & 0x02)
      {
        /*
          No background color-- initialize to black.
        */
        for (i=0; i < (ssize_t) number_planes; i++)
          background_color[i]=0;
        (void) ReadBlobByte(image);
      }
    else
      {
        /*
          Initialize background color.
        */
        p=background_color;
        for (i=0; i < (ssize_t) number_planes; i++)
          *p++=(unsigned char) ReadBlobByte(image);
      }
    if ((number_planes & 0x01) == 0)
      (void) ReadBlobByte(image);
    if (EOFBlob(image) != MagickFalse)
      ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
    if (image->matte != MagickFalse)
      number_planes++;
    number_pixels=(MagickSizeType) image->columns*image->rows;
    if ((GetBlobSize(image) == 0) || ((((MagickSizeType) number_pixels*
         number_planes*bits_per_pixel/8)/GetBlobSize(image)) > 254.0))
      ThrowRLEException(CorruptImageError,"InsufficientImageDataInFile")
    if (((MagickSizeType) number_colormaps*map_length) > GetBlobSize(image))
      ThrowRLEException(CorruptImageError,"InsufficientImageDataInFile")
    if (number_colormaps != 0)
      {
        /*
          Read image colormaps.
        */
        colormap=(unsigned char *) AcquireQuantumMemory(number_colormaps,
          3*map_length*sizeof(*colormap));
        if (colormap == (unsigned char *) NULL)
          ThrowRLEException(ResourceLimitError,"MemoryAllocationFailed");
        (void) memset(colormap,0,3*number_colormaps*map_length*
          sizeof(*colormap));
        p=colormap;
        for (i=0; i < (ssize_t) number_colormaps; i++)
          for (x=0; x < (ssize_t) map_length; x++)
          {
            *p++=(unsigned char) ScaleQuantumToChar(ScaleShortToQuantum(
              ReadBlobLSBShort(image)));
            if (EOFBlob(image) != MagickFalse)
              ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
          }
      }
    if ((flags & 0x08) != 0)
      {
        char
          *comment;

        size_t
          length;

        /*
          Read image comment.
        */
        length=ReadBlobLSBShort(image);
        if (length != 0)
          {
            comment=(char *) AcquireQuantumMemory(length,sizeof(*comment));
            if (comment == (char *) NULL)
              ThrowRLEException(ResourceLimitError,"MemoryAllocationFailed");
            (void) ReadBlob(image,length-1,(unsigned char *) comment);
            if (count != (length-1))
              {
                comment=DestroyString(comment);
                ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
              }
            comment[length-1]='\0';
            (void) SetImageProperty(image,"comment",comment);
            comment=DestroyString(comment);
            if ((length & 0x01) == 0)
              (void) ReadBlobByte(image);
          }
      }
    if (EOFBlob(image) != MagickFalse)
      ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
    if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        if (colormap != (unsigned char *) NULL)
          colormap=(unsigned char *) RelinquishMagickMemory(colormap);
        if (pixel_info != (MemoryInfo *) NULL)
          pixel_info=RelinquishVirtualMemory(pixel_info);
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    /*
      Allocate RLE pixels.
    */
    number_planes_filled=(number_planes % 2 == 0) ? number_planes :
      number_planes+1;
    if ((number_pixels*number_planes_filled) != (size_t) (number_pixels*
         number_planes_filled))
      ThrowRLEException(ResourceLimitError,"MemoryAllocationFailed");
    pixel_info=AcquireVirtualMemory(image->columns,image->rows*
      MagickMax(number_planes_filled,4)*sizeof(*pixels));
    if (pixel_info == (MemoryInfo *) NULL)
      ThrowRLEException(ResourceLimitError,"MemoryAllocationFailed");
    pixel_info_length=image->columns*image->rows*
      MagickMax(number_planes_filled,4);
    pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
    (void) memset(pixels,0,pixel_info_length);
    if ((flags & 0x01) && !(flags & 0x02))
      {
        ssize_t
          j;

        /*
          Set background color.
        */
        p=pixels;
        for (i=0; i < (ssize_t) number_pixels; i++)
        {
          if (image->matte == MagickFalse)
            for (j=0; j < (ssize_t) number_planes; j++)
              *p++=background_color[j];
          else
            {
              for (j=0; j < (ssize_t) (number_planes-1); j++)
                *p++=background_color[j];
              *p++=0;  /* initialize matte channel */
            }
        }
      }
    /*
      Read runlength-encoded image.
    */
    plane=0;
    x=0;
    y=0;
    opcode=ReadBlobByte(image);
    if (opcode == EOF)
      {
        if (number_colormaps != 0)
          colormap=(unsigned char *) RelinquishMagickMemory(colormap);
        ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
      }
    do
    {
      switch (opcode & 0x3f)
      {
        case SkipLinesOp:
        {
          operand=ReadBlobByte(image);
          if (operand == EOF)
            ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
          if (opcode & 0x40)
            {
              operand=ReadBlobLSBSignedShort(image);
              if (operand == EOF)
                ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
            }
          x=0;
          y+=operand;
          break;
        }
        case SetColorOp:
        {
          operand=ReadBlobByte(image);
          if (operand == EOF)
            ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
          plane=(unsigned char) operand;
          if (plane == 255)
            plane=(unsigned char) (number_planes-1);
          x=0;
          break;
        }
        case SkipPixelsOp:
        {
          operand=ReadBlobByte(image);
          if (operand == EOF)
            ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
          if (opcode & 0x40)
            {
              operand=ReadBlobLSBSignedShort(image);
              if (operand == EOF)
                ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
            }
          x+=operand;
          break;
        }
        case ByteDataOp:
        {
          operand=ReadBlobByte(image);
          if (operand == EOF)
            ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
          if (opcode & 0x40)
            {
              operand=ReadBlobLSBSignedShort(image);
              if (operand == EOF)
                ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
            }
          offset=(ssize_t) (((image->rows-y-1)*image->columns*number_planes)+x*
            number_planes+plane);
          operand++;
          if ((offset < 0) ||
              ((size_t) (offset+operand*number_planes) > pixel_info_length))
            ThrowRLEException(CorruptImageError,"UnableToReadImageData");
          p=pixels+offset;
          for (i=0; i < (ssize_t) operand; i++)
          {
            pixel=(unsigned char) ReadBlobByte(image);
            if ((y < (ssize_t) image->rows) &&
                ((x+i) < (ssize_t) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          if (operand & 0x01)
            (void) ReadBlobByte(image);
          x+=operand;
          break;
        }
        case RunDataOp:
        {
          operand=ReadBlobByte(image);
          if (operand == EOF)
            ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
          if (opcode & 0x40)
            {
              operand=ReadBlobLSBSignedShort(image);
              if (operand == EOF)
                ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
            }
          pixel=(unsigned char) ReadBlobByte(image);
          (void) ReadBlobByte(image);
          operand++;
          offset=(ssize_t) (((image->rows-y-1)*image->columns*number_planes)+x*
            number_planes+plane);
          if ((offset < 0) ||
              ((size_t) (offset+operand*number_planes) > pixel_info_length))
            ThrowRLEException(CorruptImageError,"UnableToReadImageData");
          p=pixels+offset;
          for (i=0; i < (ssize_t) operand; i++)
          {
            if ((y < (ssize_t) image->rows) &&
                ((x+i) < (ssize_t) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          x+=operand;
          break;
        }
        default:
          break;
      }
      opcode=ReadBlobByte(image);
      if (opcode == EOF)
        ThrowRLEException(CorruptImageError,"UnexpectedEndOfFile");
    } while (((opcode & 0x3f) != EOFOp) && (opcode != EOF));
    if (number_colormaps != 0)
      {
        MagickStatusType
          mask;

        /*
          Apply colormap affineation to image.
        */
        mask=(MagickStatusType) (map_length-1);
        p=pixels;
        x=(ssize_t) number_planes;
        if (number_colormaps == 1)
          for (i=0; i < (ssize_t) number_pixels; i++)
          {
            if (IsValidColormapIndex(image,(ssize_t) (*p & mask),&index,exception) ==
                MagickFalse)
              break;
            *p=colormap[(ssize_t) index];
            p++;
          }
        else
          if ((number_planes >= 3) && (number_colormaps >= 3))
            for (i=0; i < (ssize_t) number_pixels; i++)
              for (x=0; x < (ssize_t) number_planes; x++)
              {
                if (IsValidColormapIndex(image,(ssize_t) (x*map_length+
                    (*p & mask)),&index,exception) == MagickFalse)
                  break;
                *p=colormap[(ssize_t) index];
                p++;
              }
        if ((i < (ssize_t) number_pixels) || (x < (ssize_t) number_planes))
          ThrowRLEException(CorruptImageError,"UnableToReadImageData");
      }
    /*
      Initialize image structure.
    */
    if (number_planes >= 3)
      {
        /*
          Convert raster image to DirectClass pixel packets.
        */
        p=pixels;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelRed(q,ScaleCharToQuantum(*p++));
            SetPixelGreen(q,ScaleCharToQuantum(*p++));
            SetPixelBlue(q,ScaleCharToQuantum(*p++));
            if (image->matte != MagickFalse)
              SetPixelAlpha(q,ScaleCharToQuantum(*p++));
            q++;
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
      }
    else
      {
        /*
          Create colormap.
        */
        if (number_colormaps == 0)
          map_length=256;
        if (AcquireImageColormap(image,map_length) == MagickFalse)
          ThrowRLEException(ResourceLimitError,"MemoryAllocationFailed");
        p=colormap;
        if (number_colormaps == 1)
          for (i=0; i < (ssize_t) image->colors; i++)
          {
            /*
              Pseudocolor.
            */
            image->colormap[i].red=ScaleCharToQuantum((unsigned char) i);
            image->colormap[i].green=ScaleCharToQuantum((unsigned char) i);
            image->colormap[i].blue=ScaleCharToQuantum((unsigned char) i);
          }
        else
          if (number_colormaps > 1)
            for (i=0; i < (ssize_t) image->colors; i++)
            {
              image->colormap[i].red=ScaleCharToQuantum(*p);
              image->colormap[i].green=ScaleCharToQuantum(*(p+map_length));
              image->colormap[i].blue=ScaleCharToQuantum(*(p+map_length*2));
              p++;
            }
        p=pixels;
        if (image->matte == MagickFalse)
          {
            /*
              Convert raster image to PseudoClass pixel packets.
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetAuthenticIndexQueue(image);
              for (x=0; x < (ssize_t) image->columns; x++)
                SetPixelIndex(indexes+x,*p++);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            (void) SyncImage(image);
          }
        else
          {
            /*
              Image has a matte channel-- promote to DirectClass.
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                if (IsValidColormapIndex(image,(ssize_t) *p++,&index,exception) ==
                    MagickFalse)
                  break;
                SetPixelRed(q,image->colormap[(ssize_t) index].red);
                if (IsValidColormapIndex(image,(ssize_t) *p++,&index,exception) ==
                    MagickFalse)
                  break;
                SetPixelGreen(q,image->colormap[(ssize_t) index].green);
                if (IsValidColormapIndex(image,(ssize_t) *p++,&index,exception) ==
                    MagickFalse)
                  break;
                SetPixelBlue(q,image->colormap[(ssize_t) index].blue);
                SetPixelAlpha(q,ScaleCharToQuantum(*p++));
                q++;
              }
              if (x < (ssize_t) image->columns)
                break;
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            image->colormap=(PixelPacket *) RelinquishMagickMemory(
              image->colormap);
            image->storage_class=DirectClass;
            image->colors=0;
          }
      }
    if (number_colormaps != 0)
      colormap=(unsigned char *) RelinquishMagickMemory(colormap);
    pixel_info=RelinquishVirtualMemory(pixel_info);
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    (void) ReadBlobByte(image);
    count=ReadBlob(image,2,(unsigned char *) magick);
    if ((count != 0) && (memcmp(magick,"\122\314",2) == 0))
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            status=MagickFalse;
            break;
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  } while ((count != 0) && (memcmp(magick,"\122\314",2) == 0));
  (void) CloseBlob(image);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  return(GetFirstImageInList(image));
}