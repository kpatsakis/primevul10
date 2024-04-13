  bool operator()(const OpKernelContext* context,
                  typename TTypes<T, 4>::ConstTensor image,
                  typename TTypes<float, 2>::ConstTensor boxes,
                  typename TTypes<int32, 1>::ConstTensor box_index,
                  const string& method_name, float extrapolation_value,
                  typename TTypes<float, 4>::Tensor crops) {
    const int batch_size = image.dimension(0);
    const int image_height = image.dimension(1);
    const int image_width = image.dimension(2);

    const int num_boxes = crops.dimension(0);
    const int crop_height = crops.dimension(1);
    const int crop_width = crops.dimension(2);
    const int depth = crops.dimension(3);

    // Sharding across boxes.
    auto CropAndResizePerBox = [&](int start_box, int limit_box) {
      for (int b = start_box; b < limit_box; ++b) {
        const float y1 = boxes(b, 0);
        const float x1 = boxes(b, 1);
        const float y2 = boxes(b, 2);
        const float x2 = boxes(b, 3);

        const int32 b_in = box_index(b);
        if (!FastBoundsCheck(b_in, batch_size)) {
          continue;
        }

        const float height_scale =
            (crop_height > 1)
                ? (y2 - y1) * (image_height - 1) / (crop_height - 1)
                : 0;
        const float width_scale =
            (crop_width > 1) ? (x2 - x1) * (image_width - 1) / (crop_width - 1)
                             : 0;

        for (int y = 0; y < crop_height; ++y) {
          const float in_y = (crop_height > 1)
                                 ? y1 * (image_height - 1) + y * height_scale
                                 : 0.5 * (y1 + y2) * (image_height - 1);
          if (in_y < 0 || in_y > image_height - 1) {
            for (int x = 0; x < crop_width; ++x) {
              for (int d = 0; d < depth; ++d) {
                crops(b, y, x, d) = extrapolation_value;
              }
            }
            continue;
          }
          if (method_name == "bilinear") {

            const int top_y_index = floorf(in_y);
            const int bottom_y_index = ceilf(in_y);
            const float y_lerp = in_y - top_y_index;

            for (int x = 0; x < crop_width; ++x) {
              const float in_x = (crop_width > 1)
                                     ? x1 * (image_width - 1) + x * width_scale
                                     : 0.5 * (x1 + x2) * (image_width - 1);
              if (in_x < 0 || in_x > image_width - 1) {
                for (int d = 0; d < depth; ++d) {
                  crops(b, y, x, d) = extrapolation_value;
                }
                continue;
              }
              const int left_x_index = floorf(in_x);
              const int right_x_index = ceilf(in_x);
              const float x_lerp = in_x - left_x_index;

              for (int d = 0; d < depth; ++d) {
                const float top_left(static_cast<float>(
                    image(b_in, top_y_index, left_x_index, d)));
                const float top_right(static_cast<float>(
                    image(b_in, top_y_index, right_x_index, d)));
                const float bottom_left(static_cast<float>(
                    image(b_in, bottom_y_index, left_x_index, d)));
                const float bottom_right(static_cast<float>(
                    image(b_in, bottom_y_index, right_x_index, d)));
                const float top = top_left + (top_right - top_left) * x_lerp;
                const float bottom =
                    bottom_left + (bottom_right - bottom_left) * x_lerp;
                crops(b, y, x, d) = top + (bottom - top) * y_lerp;
              }
            }
          } else {  // method == "nearest"
            for (int x = 0; x < crop_width; ++x) {
              const float in_x = (crop_width > 1)
                                     ? x1 * (image_width - 1) + x * width_scale
                                     : 0.5 * (x1 + x2) * (image_width - 1);
              if (in_x < 0 || in_x > image_width - 1) {
                for (int d = 0; d < depth; ++d) {
                  crops(b, y, x, d) = extrapolation_value;
                }
                continue;
              }
              const int closest_x_index = roundf(in_x);
              const int closest_y_index = roundf(in_y);
              for (int d = 0; d < depth; ++d) {
                crops(b, y, x, d) = static_cast<float>(
                    image(b_in, closest_y_index, closest_x_index, d));
              }
            }
          }
        }
      }
    };

    // A rough estimation of the cost for each cropped box.
    double cost_per_pixel =
        depth * (Eigen::TensorOpCost::AddCost<float>() * 6 +
                 Eigen::TensorOpCost::MulCost<float>() * 3 +
                 Eigen::TensorOpCost::CastCost<T, float>() * 4) +
        (Eigen::TensorOpCost::AddCost<float>() * 2 +
         Eigen::TensorOpCost::AddCost<float>() * 3);
    if (method_name == "nearest") {
      cost_per_pixel = depth * Eigen::TensorOpCost::CastCost<T, float>() +
                       Eigen::TensorOpCost::AddCost<float>() * 4 +
                       Eigen::TensorOpCost::MulCost<float>() * 4;
    }
    const double cost_per_box = crop_height * crop_width * cost_per_pixel;

    const DeviceBase::CpuWorkerThreads& worker_threads =
        *(context->device()->tensorflow_cpu_worker_threads());
    Shard(worker_threads.num_threads, worker_threads.workers, num_boxes,
          cost_per_box, CropAndResizePerBox);

    return true;
  }