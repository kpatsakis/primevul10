static Image *ReadMPCImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    cache_filename[MaxTextExtent],
    id[MaxTextExtent],
    keyword[MaxTextExtent],
    *options;

  const unsigned char
    *p;

  GeometryInfo
    geometry_info;

  Image
    *image;

  int
    c;

  LinkedListInfo
    *profiles;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  MagickStatusType
    flags;

  register ssize_t
    i;

  size_t
    depth,
    length;

  ssize_t
    count;

  unsigned int
    signature;

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
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  (void) CopyMagickString(cache_filename,image->filename,MaxTextExtent-6);
  AppendImageFormat("cache",cache_filename);
  c=ReadBlobByte(image);
  if (c == EOF)
    {
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  *id='\0';
  (void) memset(keyword,0,sizeof(keyword));
  offset=0;
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    SetGeometryInfo(&geometry_info);
    profiles=(LinkedListInfo *) NULL;
    length=MaxTextExtent;
    options=AcquireString((char *) NULL);
    signature=GetMagickCoreSignature((const StringInfo *) NULL);
    image->depth=8;
    image->compression=NoCompression;
    while ((isgraph(c) != MagickFalse) && (c != (int) ':'))
    {
      register char
        *p;

      if (c == (int) '{')
        {
          char
            *comment;

          /*
            Read comment-- any text between { }.
          */
          length=MaxTextExtent;
          comment=AcquireString((char *) NULL);
          for (p=comment; comment != (char *) NULL; p++)
          {
            c=ReadBlobByte(image);
            if (c == (int) '\\')
              c=ReadBlobByte(image);
            else
              if ((c == EOF) || (c == (int) '}'))
                break;
            if ((size_t) (p-comment+1) >= length)
              {
                *p='\0';
                length<<=1;
                comment=(char *) ResizeQuantumMemory(comment,length+
                  MaxTextExtent,sizeof(*comment));
                if (comment == (char *) NULL)
                  break;
                p=comment+strlen(comment);
              }
            *p=(char) c;
          }
          if (comment == (char *) NULL)
            {
              options=DestroyString(options);
              ThrowReaderException(ResourceLimitError,
                "MemoryAllocationFailed");
            }
          *p='\0';
          (void) SetImageProperty(image,"comment",comment);
          comment=DestroyString(comment);
          c=ReadBlobByte(image);
        }
      else
        if (isalnum(c) != MagickFalse)
          {
            /*
              Get the keyword.
            */
            length=MaxTextExtent-1;
            p=keyword;
            do
            {
              if (c == (int) '=')
                break;
              if ((size_t) (p-keyword) < (MaxTextExtent-1))
                *p++=(char) c;
              c=ReadBlobByte(image);
            } while (c != EOF);
            *p='\0';
            p=options;
            while (isspace((int) ((unsigned char) c)) != 0)
              c=ReadBlobByte(image);
            if (c == (int) '=')
              {
                /*
                  Get the keyword value.
                */
                c=ReadBlobByte(image);
                while ((c != (int) '}') && (c != EOF))
                {
                  if ((size_t) (p-options+1) >= length)
                    {
                      *p='\0';
                      length<<=1;
                      options=(char *) ResizeQuantumMemory(options,length+
                        MaxTextExtent,sizeof(*options));
                      if (options == (char *) NULL)
                        break;
                      p=options+strlen(options);
                    }
                  *p++=(char) c;
                  c=ReadBlobByte(image);
                  if (c == '\\')
                    {
                      c=ReadBlobByte(image);
                      if (c == (int) '}')
                        {
                          *p++=(char) c;
                          c=ReadBlobByte(image);
                        }
                    }
                  if (*options != '{')
                    if (isspace((int) ((unsigned char) c)) != 0)
                      break;
                }
                if (options == (char *) NULL)
                  ThrowReaderException(ResourceLimitError,
                    "MemoryAllocationFailed");
              }
            *p='\0';
            if (*options == '{')
              (void) CopyMagickString(options,options+1,strlen(options));
            /*
              Assign a value to the specified keyword.
            */
            switch (*keyword)
            {
              case 'b':
              case 'B':
              {
                if (LocaleCompare(keyword,"background-color") == 0)
                  {
                    (void) QueryColorDatabase(options,&image->background_color,
                      exception);
                    break;
                  }
                if (LocaleCompare(keyword,"blue-primary") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.blue_primary.x=geometry_info.rho;
                    image->chromaticity.blue_primary.y=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->chromaticity.blue_primary.y=
                        image->chromaticity.blue_primary.x;
                    break;
                  }
                if (LocaleCompare(keyword,"border-color") == 0)
                  {
                    (void) QueryColorDatabase(options,&image->border_color,
                      exception);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'c':
              case 'C':
              {
                if (LocaleCompare(keyword,"class") == 0)
                  {
                    ssize_t
                      storage_class;

                    storage_class=ParseCommandOption(MagickClassOptions,
                      MagickFalse,options);
                    if (storage_class < 0)
                      break;
                    image->storage_class=(ClassType) storage_class;
                    break;
                  }
                if (LocaleCompare(keyword,"colors") == 0)
                  {
                    image->colors=StringToUnsignedLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"colorspace") == 0)
                  {
                    ssize_t
                      colorspace;

                    colorspace=ParseCommandOption(MagickColorspaceOptions,
                      MagickFalse,options);
                    if (colorspace < 0)
                      break;
                    image->colorspace=(ColorspaceType) colorspace;
                    break;
                  }
                if (LocaleCompare(keyword,"compression") == 0)
                  {
                    ssize_t
                      compression;

                    compression=ParseCommandOption(MagickCompressOptions,
                      MagickFalse,options);
                    if (compression < 0)
                      break;
                    image->compression=(CompressionType) compression;
                    break;
                  }
                if (LocaleCompare(keyword,"columns") == 0)
                  {
                    image->columns=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'd':
              case 'D':
              {
                if (LocaleCompare(keyword,"delay") == 0)
                  {
                    image->delay=StringToUnsignedLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"depth") == 0)
                  {
                    image->depth=StringToUnsignedLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"dispose") == 0)
                  {
                    ssize_t
                      dispose;

                    dispose=ParseCommandOption(MagickDisposeOptions,MagickFalse,
                      options);
                    if (dispose < 0)
                      break;
                    image->dispose=(DisposeType) dispose;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'e':
              case 'E':
              {
                if (LocaleCompare(keyword,"endian") == 0)
                  {
                    ssize_t
                      endian;

                    endian=ParseCommandOption(MagickEndianOptions,MagickFalse,
                      options);
                    if (endian < 0)
                      break;
                    image->endian=(EndianType) endian;
                    break;
                  }
                if (LocaleCompare(keyword,"error") == 0)
                  {
                    image->error.mean_error_per_pixel=StringToDouble(options,
                      (char **) NULL);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'g':
              case 'G':
              {
                if (LocaleCompare(keyword,"gamma") == 0)
                  {
                    image->gamma=StringToDouble(options,(char **) NULL);
                    break;
                  }
                if (LocaleCompare(keyword,"green-primary") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.green_primary.x=geometry_info.rho;
                    image->chromaticity.green_primary.y=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->chromaticity.green_primary.y=
                        image->chromaticity.green_primary.x;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'i':
              case 'I':
              {
                if (LocaleCompare(keyword,"id") == 0)
                  {
                    (void) CopyMagickString(id,options,MaxTextExtent);
                    break;
                  }
                if (LocaleCompare(keyword,"iterations") == 0)
                  {
                    image->iterations=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'm':
              case 'M':
              {
                if (LocaleCompare(keyword,"magick-signature") == 0)
                  {
                    signature=(unsigned int) StringToUnsignedLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"matte") == 0)
                  {
                    ssize_t
                      matte;

                    matte=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                      options);
                    if (matte < 0)
                      break;
                    image->matte=(MagickBooleanType) matte;
                    break;
                  }
                if (LocaleCompare(keyword,"matte-color") == 0)
                  {
                    (void) QueryColorDatabase(options,&image->matte_color,
                      exception);
                    break;
                  }
                if (LocaleCompare(keyword,"maximum-error") == 0)
                  {
                    image->error.normalized_maximum_error=StringToDouble(
                      options,(char **) NULL);
                    break;
                  }
                if (LocaleCompare(keyword,"mean-error") == 0)
                  {
                    image->error.normalized_mean_error=StringToDouble(options,
                      (char **) NULL);
                    break;
                  }
                if (LocaleCompare(keyword,"montage") == 0)
                  {
                    (void) CloneString(&image->montage,options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'o':
              case 'O':
              {
                if (LocaleCompare(keyword,"opaque") == 0)
                  {
                    ssize_t
                      matte;

                    matte=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                      options);
                    if (matte < 0)
                      break;
                    image->matte=(MagickBooleanType) matte;
                    break;
                  }
                if (LocaleCompare(keyword,"orientation") == 0)
                  {
                    ssize_t
                      orientation;

                    orientation=ParseCommandOption(MagickOrientationOptions,
                      MagickFalse,options);
                    if (orientation < 0)
                      break;
                    image->orientation=(OrientationType) orientation;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'p':
              case 'P':
              {
                if (LocaleCompare(keyword,"page") == 0)
                  {
                    char
                      *geometry;

                    geometry=GetPageGeometry(options);
                    (void) ParseAbsoluteGeometry(geometry,&image->page);
                    geometry=DestroyString(geometry);
                    break;
                  }
                if (LocaleCompare(keyword,"pixel-intensity") == 0)
                  {
                    ssize_t
                      intensity;

                    intensity=ParseCommandOption(MagickPixelIntensityOptions,
                      MagickFalse,options);
                    if (intensity < 0)
                      break;
                    image->intensity=(PixelIntensityMethod) intensity;
                    break;
                  }
                if (LocaleCompare(keyword,"profile") == 0)
                  {
                    if (profiles == (LinkedListInfo *) NULL)
                      profiles=NewLinkedList(0);
                    (void) AppendValueToLinkedList(profiles,
                      AcquireString(options));
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'q':
              case 'Q':
              {
                if (LocaleCompare(keyword,"quality") == 0)
                  {
                    image->quality=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'r':
              case 'R':
              {
                if (LocaleCompare(keyword,"red-primary") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.red_primary.x=geometry_info.rho;
                    if ((flags & SigmaValue) != 0)
                      image->chromaticity.red_primary.y=geometry_info.sigma;
                    break;
                  }
                if (LocaleCompare(keyword,"rendering-intent") == 0)
                  {
                    ssize_t
                      rendering_intent;

                    rendering_intent=ParseCommandOption(MagickIntentOptions,
                      MagickFalse,options);
                    if (rendering_intent < 0)
                      break;
                    image->rendering_intent=(RenderingIntent) rendering_intent;
                    break;
                  }
                if (LocaleCompare(keyword,"resolution") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->x_resolution=geometry_info.rho;
                    image->y_resolution=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->y_resolution=image->x_resolution;
                    break;
                  }
                if (LocaleCompare(keyword,"rows") == 0)
                  {
                    image->rows=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 's':
              case 'S':
              {
                if (LocaleCompare(keyword,"scene") == 0)
                  {
                    image->scene=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 't':
              case 'T':
              {
                if (LocaleCompare(keyword,"ticks-per-second") == 0)
                  {
                    image->ticks_per_second=(ssize_t) StringToLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"tile-offset") == 0)
                  {
                    char
                      *geometry;

                    geometry=GetPageGeometry(options);
                    (void) ParseAbsoluteGeometry(geometry,&image->tile_offset);
                    geometry=DestroyString(geometry);
                  }
                if (LocaleCompare(keyword,"type") == 0)
                  {
                    ssize_t
                      type;

                    type=ParseCommandOption(MagickTypeOptions,MagickFalse,
                      options);
                    if (type < 0)
                      break;
                    image->type=(ImageType) type;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'u':
              case 'U':
              {
                if (LocaleCompare(keyword,"units") == 0)
                  {
                    ssize_t
                      units;

                    units=ParseCommandOption(MagickResolutionOptions,MagickFalse,
                      options);
                    if (units < 0)
                      break;
                    image->units=(ResolutionType) units;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'w':
              case 'W':
              {
                if (LocaleCompare(keyword,"white-point") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.white_point.x=geometry_info.rho;
                    image->chromaticity.white_point.y=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->chromaticity.white_point.y=
                        image->chromaticity.white_point.x;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              default:
              {
                (void) SetImageProperty(image,keyword,options);
                break;
              }
            }
          }
        else
          c=ReadBlobByte(image);
      while (isspace((int) ((unsigned char) c)) != 0)
        c=ReadBlobByte(image);
    }
    options=DestroyString(options);
    (void) ReadBlobByte(image);
    /*
      Verify that required image information is defined.
    */
    if ((LocaleCompare(id,"MagickCache") != 0) || (image->depth > 128) ||
        (image->storage_class == UndefinedClass) ||
        (image->compression == UndefinedCompression) || (image->columns == 0) ||
        (image->rows == 0))
      {
        if (profiles != (LinkedListInfo *) NULL)
          profiles=DestroyLinkedList(profiles,RelinquishMagickMemory);
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      }
    if (signature != GetMagickCoreSignature((const StringInfo *) NULL))
      {
        if (profiles != (LinkedListInfo *) NULL)
          profiles=DestroyLinkedList(profiles,RelinquishMagickMemory);
        ThrowReaderException(CacheError,"IncompatibleAPI");
      }
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Image directory.
        */
        length=MaxTextExtent;
        image->directory=AcquireString((char *) NULL);
        p=image->directory;
        do
        {
          *p='\0';
          if ((strlen(image->directory)+MaxTextExtent) >= length)
            {
              /*
                Allocate more memory for the image directory.
              */
              length<<=1;
              image->directory=(char *) ResizeQuantumMemory(image->directory,
                length+MaxTextExtent,sizeof(*image->directory));
              if (image->directory == (char *) NULL)
                {
                  if (profiles != (LinkedListInfo *) NULL)
                    profiles=DestroyLinkedList(profiles,RelinquishMagickMemory);
                  ThrowReaderException(CorruptImageError,
                    "UnableToReadImageData");
                }
              p=image->directory+strlen(image->directory);
            }
          c=ReadBlobByte(image);
          if (c == EOF)
            break;
          *p++=(char) c;
        } while (c != (int) '\0');
      }
    if (profiles != (LinkedListInfo *) NULL)
      {
        const char
          *name;

        StringInfo
          *profile;

        /*
          Read image profile blobs.
        */
        ResetLinkedListIterator(profiles);
        name=(const char *) GetNextValueInLinkedList(profiles);
        while (name != (const char *) NULL)
        {
          length=ReadBlobMSBLong(image);
          if ((MagickSizeType) length > GetBlobSize(image))
            break;
          profile=AcquireStringInfo(length);
          if (profile == (StringInfo *) NULL)
            break;
          count=ReadBlob(image,length,GetStringInfoDatum(profile));
          if (count != (ssize_t) length)
            {
              profile=DestroyStringInfo(profile);
              break;
            }
          status=SetImageProfile(image,name,profile);
          profile=DestroyStringInfo(profile);
          if (status == MagickFalse)
            break;
          name=(const char *) GetNextValueInLinkedList(profiles);
        }
        profiles=DestroyLinkedList(profiles,RelinquishMagickMemory);
      }
    depth=GetImageQuantumDepth(image,MagickFalse);
    if (image->storage_class == PseudoClass)
      {
        size_t
          packet_size;

        unsigned char
          *colormap;

        /*
          Create image colormap.
        */
        packet_size=(size_t) (3UL*depth/8UL);
        if ((MagickSizeType) (packet_size*image->colors) > GetBlobSize(image))
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        image->colormap=(PixelPacket *) AcquireQuantumMemory(image->colors+1,
          sizeof(*image->colormap));
        if (image->colormap == (PixelPacket *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        if (image->colors != 0)
          {
            /*
              Read image colormap from file.
            */
            colormap=(unsigned char *) AcquireQuantumMemory(image->colors,
              packet_size*sizeof(*colormap));
            if (colormap == (unsigned char *) NULL)
              ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
            count=ReadBlob(image,packet_size*image->colors,colormap);
            if (count != (ssize_t) (packet_size*image->colors))
              {
                colormap=(unsigned char *) RelinquishMagickMemory(colormap);
                ThrowReaderException(CorruptImageError,
                  "InsufficientImageDataInFile");
              }
            p=colormap;
            switch (depth)
            {
              default:
                colormap=(unsigned char *) RelinquishMagickMemory(colormap);
                ThrowReaderException(CorruptImageError,
                  "ImageDepthNotSupported");
              case 8:
              {
                unsigned char
                  pixel;

                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  p=PushCharPixel(p,&pixel);
                  image->colormap[i].red=ScaleCharToQuantum(pixel);
                  p=PushCharPixel(p,&pixel);
                  image->colormap[i].green=ScaleCharToQuantum(pixel);
                  p=PushCharPixel(p,&pixel);
                  image->colormap[i].blue=ScaleCharToQuantum(pixel);
                }
                break;
              }
              case 16:
              {
                unsigned short
                  pixel;

                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  p=PushShortPixel(MSBEndian,p,&pixel);
                  image->colormap[i].red=ScaleShortToQuantum(pixel);
                  p=PushShortPixel(MSBEndian,p,&pixel);
                  image->colormap[i].green=ScaleShortToQuantum(pixel);
                  p=PushShortPixel(MSBEndian,p,&pixel);
                  image->colormap[i].blue=ScaleShortToQuantum(pixel);
                }
                break;
              }
              case 32:
              {
                unsigned int
                  pixel;

                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  p=PushLongPixel(MSBEndian,p,&pixel);
                  image->colormap[i].red=ScaleLongToQuantum(pixel);
                  p=PushLongPixel(MSBEndian,p,&pixel);
                  image->colormap[i].green=ScaleLongToQuantum(pixel);
                  p=PushLongPixel(MSBEndian,p,&pixel);
                  image->colormap[i].blue=ScaleLongToQuantum(pixel);
                }
                break;
              }
            }
            colormap=(unsigned char *) RelinquishMagickMemory(colormap);
          }
      }
    if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if ((AcquireMagickResource(WidthResource,image->columns) == MagickFalse) ||
        (AcquireMagickResource(HeightResource,image->rows) == MagickFalse))
      ThrowReaderException(ImageError,"WidthOrHeightExceedsLimit");
    /*
      Attach persistent pixel cache.
    */
    status=PersistPixelCache(image,cache_filename,MagickTrue,&offset,exception);
    if (status == MagickFalse)
      {
        status=SetImageExtent(image,image->columns,image->rows);
        ThrowReaderException(CacheError,"UnableToPersistPixelCache");
      }
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    do
    {
      c=ReadBlobByte(image);
    } while ((isgraph(c) == MagickFalse) && (c != EOF));
    if (c != EOF)
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            image=DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  } while (c != EOF);
  (void) CloseBlob(image);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  return(GetFirstImageInList(image));
}