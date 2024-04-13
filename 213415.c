  static void Launch(OpKernelContext* context, const Tensor& in_x,
                     const Tensor& in_y, bool adjoint, bool lower,
                     const MatMulBCast& bcast, Tensor* out) {
    // Number of banded matrix triangular solves i.e. size of the batch.
    const int64 batch_size = bcast.output_batch_size();
    const int64 cost_per_unit =
        in_x.dim_size(1) * in_x.dim_size(2) * in_y.dim_size(2);
    auto worker_threads = *(context->device()->tensorflow_cpu_worker_threads());

    using Matrix =
        Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using ConstMatrixMap = Eigen::Map<const Matrix>;
    using RealScalar = typename Eigen::NumTraits<Scalar>::Real;
    // Check diagonal before doing any solves. This is the first row in the
    // lower case and else is the last row.
    auto matrix = ConstMatrixMap(in_x.flat<Scalar>().data(), in_x.dim_size(1),
                                 in_x.dim_size(2));
    RealScalar min_abs_pivot;
    if (lower) {
      min_abs_pivot = matrix.row(0).cwiseAbs().minCoeff();
    } else {
      min_abs_pivot = matrix.row(in_x.dim_size(1) - 1).cwiseAbs().minCoeff();
    }
    OP_REQUIRES(context, min_abs_pivot > RealScalar(0),
                errors::InvalidArgument("Input matrix is not invertible."));

    Shard(worker_threads.num_threads, worker_threads.workers, batch_size,
          cost_per_unit,
          [&in_x, &in_y, adjoint, lower, &bcast, out](int64 start,
                                                      int64 limit) {
            SequentialBandedTriangularSolveKernel<Scalar>::Run(
                in_x, in_y, lower, adjoint, bcast, out, start, limit);
          });
  }