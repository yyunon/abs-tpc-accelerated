//
// Created by Fabian Nonnenmacher on 11.05.20.
//
#include <stdio.h> /* defines FILENAME_MAX */
#include <time.h>
#include "arrow/api.h"
#include <arrow/dataset/api.h>
#include <arrow/filesystem/api.h>

#include "FletcherParquetReader.h"

__thread uint32_t tpc::instance_index;
__thread tpc::internal::WorkStealingTaskQueue *tpc::internal::ThreadPool::local_worker_queue;
//std::condition_variable tpc::cv;
//std::mutex tpc::mmio_mtx;
//static bool tpc::block_mmio;
namespace tpc
{
  //FletcherParquetReader::FletcherParquetReader(std::shared_ptr<arrow::MemoryPool> &memory_pool,
  FletcherParquetReader::FletcherParquetReader(PlatformWrapper * platform_w,
                                               uint64_t group_length,
                                               PtoaRegs ** regs,
                                               const std::string &file_name,
                                               const std::shared_ptr<arrow::Schema> &schema_file,
                                               const std::shared_ptr<arrow::Schema> &schema_out,
                                               int num_rows)
  {
    double t_total, t_compute,t_meta;

    struct timespec start_total, finish_total, start_meta, finish_meta, start_compute, finish_compute;
    //clock_gettime(CLOCK_MONOTONIC, &start_total);
    //auto t_total_start = std::chrono::high_resolution_clock::now();
    //std::cout << "Native started... \n";
    int num_values = num_rows;
    //clock_gettime(CLOCK_MONOTONIC, &start_meta);
    //std::shared_ptr<FileReader> metadataParser = FileReader::Make(file_name, std::move(schema_out), true);
    //auto regs = metadataParser->offsetsPreloaded1SF();
    //PtoaRegs ** regs = new PtoaRegs*[metadataParser->num_row_groups];
    //for(int i = 0; i < metadataParser->num_row_groups; ++i)
    //  regs[i] = new PtoaRegs[4];
    
    //std::cout << "Regs initialized... \n";
    //ASSERT_FLETCHER_OK(metadataParser->readChunks(regs));
    //clock_gettime(CLOCK_MONOTONIC, &finish_meta);
    //std::vector<std::vector<PtoaRegs>> regs = metadataParser->readChunks();
    //column_scheduler = std::atomic_load(&(platform_w->column_scheduler));

    // As soon as you create column scheduler, you submit the task
    //auto t_compute_start = std::chrono::high_resolution_clock::now();
    clock_gettime(CLOCK_MONOTONIC, &start_compute);
    ASSERT_FLETCHER_OK(platform_w->Submit(group_length, regs));
    clock_gettime(CLOCK_MONOTONIC, &finish_compute);

    //clock_gettime(CLOCK_MONOTONIC, &finish_total);

    //t_total = (finish_total.tv_sec - start_total.tv_sec);
    //t_total += (finish_total.tv_nsec - start_total.tv_nsec) / 1000000000.0;
    t_compute = (finish_compute.tv_sec - start_compute.tv_sec);
    t_compute += (finish_compute.tv_nsec - start_compute.tv_nsec) / 1000000000.0;
    //t_meta = (finish_meta.tv_sec - start_meta.tv_sec);
    //t_meta += (finish_meta.tv_nsec - start_meta.tv_nsec) / 1000000000.0;
    //auto t_meta = std::chrono::duration_cast<std::chrono::microseconds>(t_meta_end - t_meta_start).count();
    //auto t_total = std::chrono::duration_cast<std::chrono::microseconds>(t_total_end - t_total_start).count();
    //auto t_compute = std::chrono::duration_cast<std::chrono::microseconds>(t_compute_end - t_compute_start).count();


    //platform_w->duration_total += t_total;
    platform_w->duration_compute += t_compute;
    //platform_w->duration_meta += t_meta;
    //std::cout << "Native runtime : \t" << platform_w->duration << " us" << std::endl;
    //printf("Native runtime\n");
    //printf("%.6f,%.6f\n", platform_w->duration_total, platform_w->duration_compute);
    //printf("%.6f,%.6f\n", t_total, t_compute);
    printf("%.6f,%.6f\n", platform_w->duration_compute);
    printf("%.6f,%.6f\n", t_compute);

  }
  
  double FletcherParquetReader::Next(PlatformWrapper* platform_w)
  {
    while(!platform_w->hasNext()) {}
    return platform_w->Next();
  }

  bool FletcherParquetReader::hasNext(PlatformWrapper* platform_w)
  {
    return platform_w->hasNext();
  }

}
