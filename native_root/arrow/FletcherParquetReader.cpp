//
// Created by Fabian Nonnenmacher on 11.05.20.
//
#include <stdio.h> /* defines FILENAME_MAX */
#include "arrow/api.h"
#include <arrow/dataset/api.h>
#include <arrow/filesystem/api.h>

#include "FletcherParquetReader.h"

__thread uint32_t tpc::instance_index;
__thread tpc::internal::WorkStealingTaskQueue *tpc::internal::ThreadPool::local_worker_queue;
std::condition_variable tpc::cv;
std::mutex tpc::mmio_mtx;
//static bool tpc::block_mmio;

namespace tpc
{
  //FletcherParquetReader::FletcherParquetReader(std::shared_ptr<arrow::MemoryPool> &memory_pool,
  FletcherParquetReader::FletcherParquetReader(PlatformWrapper * platform_w,
                                               const std::string &file_name,
                                               const std::shared_ptr<arrow::Schema> &schema_file,
                                               const std::shared_ptr<arrow::Schema> &schema_out,
                                               int num_rows)
  {
    std::cout << "Native started... \n";
    int num_values = num_rows;
    auto t_meta_start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<FileReader> metadataParser = FileReader::Make(file_name, std::move(schema_out), true);
    //auto regs = metadataParser->offsetsPreloaded1SF();
    PtoaRegs ** regs = new PtoaRegs*[metadataParser->num_row_groups];
    for(int i = 0; i < metadataParser->num_row_groups; ++i)
      regs[i] = new PtoaRegs[4];
    
    std::cout << "Regs initialized... \n";
    ASSERT_FLETCHER_OK(metadataParser->readChunks(regs));
    //std::vector<std::vector<PtoaRegs>> regs = metadataParser->readChunks();
    //column_scheduler = std::atomic_load(&(platform_w->column_scheduler));
    auto t_meta_end = std::chrono::high_resolution_clock::now();

    // As soon as you create column scheduler, you submit the task
    ASSERT_FLETCHER_OK(platform_w->Submit(metadataParser->num_row_groups, regs));
    auto t_meta = std::chrono::duration_cast<std::chrono::microseconds>(t_meta_end - t_meta_start).count();
    std::cout << "Native runtime : \t" << t_meta << "us" << std::endl;

    for(int i = 0; i <metadataParser->num_row_groups; ++i)
      delete[] regs[i];
    delete[] regs; 
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
