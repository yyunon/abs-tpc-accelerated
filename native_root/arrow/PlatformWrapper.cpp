//
// Created by Fabian Nonnenmacher on 11.05.20.
//
#include <stdio.h> /* defines FILENAME_MAX */
#include "arrow/api.h"
#include <arrow/dataset/api.h>
#include <arrow/filesystem/api.h>

#include "PlatformWrapper.h"

namespace tpc
{
  //FletcherParquetReader::FletcherParquetReader(std::shared_ptr<arrow::MemoryPool> &memory_pool,
  PlatformWrapper::PlatformWrapper() 
  {
    //printf("Creating platform\n");
    duration_total = 0;
    duration_meta = 0;
    duration_compute = 0;
    ASSERT_FLETCHER_OK(fletcher::Platform::Make("snap", &platform, false));
    ASSERT_FLETCHER_OK(platform->Init());
    ASSERT_FLETCHER_OK(ColumnScheduler::Make(&column_scheduler, platform));
    //column_scheduler = new ColumnScheduler(platform);
    //printf("Created platform\n");
  }
  
  fletcher::Status PlatformWrapper::Submit(int num_row_groups,PtoaRegs** regs)
  {
    ASSERT_FLETCHER_OK(column_scheduler->Submit(num_row_groups,regs));
    return fletcher::Status::OK();
  }

  double PlatformWrapper::Next()
  {
    while(!column_scheduler->hasNext()) {}
    return column_scheduler->Next();
  }

  bool PlatformWrapper::hasNext()
  {
    return column_scheduler->hasNext();
  }

}
