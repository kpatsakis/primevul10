  void SpatialMaxPoolGradGrad(OpKernelContext* context, Tensor* bottom_diff,
                              const Tensor& tensor_in, const Tensor& tensor_out,
                              const Tensor& top_diff,
                              const PoolParameters& params,
                              const Padding& padding) {
    typedef Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
        ConstEigenMatrixMap;
    typedef Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
        EigenMatrixMap;

    ConstEigenMatrixMap in_mat(
        tensor_in.flat<T>().data(), params.depth,
        params.tensor_in_cols * params.tensor_in_rows * params.tensor_in_batch);
    ConstEigenMatrixMap out_mat(
        tensor_out.flat<T>().data(), params.depth,
        params.out_width * params.out_height * params.tensor_in_batch);
    ConstEigenMatrixMap top_diff_mat(
        top_diff.flat<T>().data(), params.depth,
        params.tensor_in_cols * params.tensor_in_rows * params.tensor_in_batch);
    EigenMatrixMap bottom_diff_mat(
        bottom_diff->flat<T>().data(), params.depth,
        params.out_width * params.out_height * params.tensor_in_batch);

    const DeviceBase::CpuWorkerThreads& worker_threads =
        *(context->device()->tensorflow_cpu_worker_threads());

    // The following code basically does the following:
    // 1. Flattens the input, output, top_diff and bottom_diff tensors into
    //    two dimensional arrays.
    //    tensor_in_as_matrix:
    //      depth by (tensor_in_cols * tensor_in_rows * tensor_in_batch)
    //    tensor_out_as_matrix:
    //      depth by (out_width * out_height * tensor_in_batch)
    //    top_diff_as_matrix:
    //      depth by (tensor_in_cols * tensor_in_rows * tensor_in_batch)
    //    bottom_diff_as_matrix:
    //      depth by (out_width * out_height * tensor_in_batch)
    //
    // 2. Walks through the set of columns in the flattened
    //    tensor_in_as_matrix, tensor_out_as_matrix, top_diff_as_matrix
    //    and updates the column(s) corresponding to the maximum values in
    //    tensor_out_as_matrix with the corresponding values in
    //    top_diff_as_matrix.
    auto shard = [&params, &in_mat, &out_mat, &top_diff_mat, &bottom_diff_mat](
                     int64 start, int64 limit) {
      const int32 depth = params.depth;
      const int32 in_rows = params.tensor_in_rows;
      const int32 in_cols = params.tensor_in_cols;
      const int32 pad_top = params.pad_top;
      const int32 pad_left = params.pad_left;
      const int32 window_rows = params.window_rows;
      const int32 window_cols = params.window_cols;
      const int32 row_stride = params.row_stride;
      const int32 col_stride = params.col_stride;
      const int32 out_height = params.out_height;
      const int32 out_width = params.out_width;

      {
        // Initializes the output grad backprop tensor with 0.
        const int32 output_image_size = out_height * out_width * params.depth;
        EigenMatrixMap bottom_diff_shard(
            bottom_diff_mat.data() + start * output_image_size, 1,
            (limit - start) * output_image_size);
        bottom_diff_shard.setZero();
      }

      for (int b = start; b < limit; ++b) {
        for (int ph = 0; ph < out_height; ++ph) {
          for (int pw = 0; pw < out_width; ++pw) {
            // (h_start, h_end) * (w_start, w_end) is the range that the input
            // vector projects to.
            int h_start = ph * row_stride - pad_top;
            const int h_end = std::min(h_start + window_rows, in_rows);
            int w_start = pw * col_stride - pad_left;
            const int w_end = std::min(w_start + window_cols, in_cols);
            h_start = std::max(h_start, 0);
            w_start = std::max(w_start, 0);
            const int out_index = (b * out_height + ph) * out_width + pw;
            // Find value corresponding to the input maximum in top_diff.
            for (int d = 0; d < depth; ++d) {
              const T& output_ref = out_mat.coeffRef(d, out_index);
              bool should_stop = false;
              for (int h = h_start; h < h_end && !should_stop; ++h) {
                for (int w = w_start; w < w_end && !should_stop; ++w) {
                  const int in_index = (b * in_rows + h) * in_cols + w;
                  const T& input_ref = in_mat.coeffRef(d, in_index);
                  if (output_ref == input_ref) {
                    T& bottom_diff_ref = bottom_diff_mat.coeffRef(d, out_index);
                    bottom_diff_ref = top_diff_mat.coeffRef(d, in_index);
                    should_stop = true;
                  }
                }
              }
            }
          }
        }
      }
    };

    const int64 shard_cost = params.out_width * params.out_height *
                             params.depth * params.window_rows *
                             params.window_cols;
    Shard(worker_threads.num_threads, worker_threads.workers,
          params.tensor_in_batch, shard_cost, shard);
  }