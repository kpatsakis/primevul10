static Image *ReadHEICImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  heif_item_id
    exif_id;

  Image
    *image;

  int
    count,
    stride_y,
    stride_cb,
    stride_cr;

  MagickBooleanType
    status;

  MagickSizeType
    length;

  ssize_t
    y;

  struct heif_context
    *heif_context;

  struct heif_error
    error;

  struct heif_image
    *heif_image;

  struct heif_image_handle
    *image_handle;

  uint8_t
    *p_y,
    *p_cb,
    *p_cr;

  void
    *file_data;

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
  length=GetBlobSize(image);
  file_data=AcquireMagickMemory(length);
  if (file_data == (void *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  if (ReadBlob(image,length,file_data) != (ssize_t) length)
    {
      file_data=RelinquishMagickMemory(file_data);
      ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
    }
  /*
    Decode HEIF file
  */
  heif_context=heif_context_alloc();
  error=heif_context_read_from_memory(heif_context,file_data,length,NULL);
  file_data=RelinquishMagickMemory(file_data);
  if (IsHeifSuccess(&error,image) == MagickFalse)
    {
      heif_context_free(heif_context);
      return(DestroyImageList(image));
    }
  image_handle=(struct heif_image_handle *) NULL;
  error=heif_context_get_primary_image_handle(heif_context,&image_handle);
  if (IsHeifSuccess(&error,image) == MagickFalse)
    {
      heif_context_free(heif_context);
      return(DestroyImageList(image));
    }
  /*
    Read Exif data from HEIC file
  */
  count=heif_image_handle_get_list_of_metadata_block_IDs(image_handle,"Exif",
    &exif_id,1);
  if (count > 0)
    {
      size_t
        exif_size;

      void
        *exif_buffer;

      exif_size=heif_image_handle_get_metadata_size(image_handle,exif_id);
      if (exif_size > GetBlobSize(image))
        {
          heif_image_handle_release(image_handle);
          heif_context_free(heif_context);
          ThrowReaderException(CorruptImageError,
            "InsufficientImageDataInFile");
        }
      exif_buffer=AcquireMagickMemory(exif_size);
      error=heif_image_handle_get_metadata(image_handle,exif_id,exif_buffer);
      if (error.code == 0)
        {
          StringInfo
            *profile;

          profile=BlobToStringInfo(exif_buffer,exif_size);
          SetImageProfile(image,"exif",profile);
          profile=DestroyStringInfo(profile);
      }
      exif_buffer=RelinquishMagickMemory(exif_buffer);
  }
  /*
    Set image size
   */
  image->depth=8;
  image->columns=(size_t) heif_image_handle_get_width(image_handle);
  image->rows=(size_t) heif_image_handle_get_height(image_handle);
  if (image_info->ping != MagickFalse)
    {
      image->colorspace=YCbCrColorspace;
      heif_image_handle_release(image_handle);
      heif_context_free(heif_context);
      return(GetFirstImageInList(image));
    }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      heif_image_handle_release(image_handle);
      heif_context_free(heif_context);
      return(DestroyImageList(image));
    }
  /*
    Copy HEIF image into ImageMagick data structures
  */
  (void) SetImageColorspace(image,YCbCrColorspace);
  error=heif_decode_image(image_handle,&heif_image,heif_colorspace_YCbCr,
    heif_chroma_420,NULL);
  if (IsHeifSuccess(&error,image) == MagickFalse)
    {
      heif_image_handle_release(image_handle);
      heif_context_free(heif_context);
      return(DestroyImageList(image));
    }
  p_y=heif_image_get_plane(heif_image,heif_channel_Y,&stride_y);
  p_cb=heif_image_get_plane(heif_image,heif_channel_Cb,&stride_cb);
  p_cr=heif_image_get_plane(heif_image,heif_channel_Cr,&stride_cr);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    PixelPacket
      *q;

    register ssize_t
      x;

    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      SetPixelRed(q,ScaleCharToQuantum(p_y[y*stride_y + x]));
      SetPixelGreen(q,ScaleCharToQuantum(p_cb[(y/2)*stride_cb + x/2]));
      SetPixelBlue(q,ScaleCharToQuantum(p_cr[(y/2)*stride_cr + x/2]));
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
  heif_image_release(heif_image);
  heif_image_handle_release(image_handle);
  heif_context_free(heif_context);
  return(GetFirstImageInList(image));
}