gdImagePtr get_valid_image_resource(const Resource& image) {
  auto img_res = dyn_cast_or_null<Image>(image);
  if (!img_res || !img_res->get()) {
    raise_warning("supplied resource is not a valid Image resource");
    return nullptr;
  }
  return img_res->get();
}