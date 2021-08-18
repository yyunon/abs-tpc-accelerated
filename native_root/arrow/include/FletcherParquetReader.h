#pragma once

#include "FileReader.h"
#include "Tpch.h"
#include "ColumnScheduler.h"
#include "PlatformWrapper.h"
#include "Util.h"

#include <iostream>
#include <arrow/api.h>
#include <mutex>
#include <parquet/arrow/reader.h>

#include "fletcher/api.h"

namespace tpc
{
    class FletcherParquetReader
    {
    private:
        //keeping all this objects as shared_ptr field makes, sure that they are not deleted while the FletcherParquetReader is still active
        //std::shared_ptr<ColumnScheduler> column_scheduler;
        //std::vector<std::vector<uint64_t>> base_offsets[NUM_OF_HW_INSTANCES];
        //tpc::Platform_Wrapper thread_wrapper;
        //std::vector<std::shared_ptr<fletcher::Context>> contexts;
        //std::vector<fletcher::Kernel> kernels;
        //std::vector<std::shared_ptr<tpc::Tpch>> instances;
        //std::shared_ptr<fletcher::Platform> platform;
        //std::shared_ptr<FileReader> metadataParser;
        //int num_values;
        //int num_of_row_groups;
        //std::vector<std::vector<PtoaRegs>> regs;
        //std::shared_ptr<arrow::MemoryPool> pool_;
        long long duration = 0;

    public:
        //FletcherParquetReader(std::shared_ptr<arrow::MemoryPool> &memory_pool,
        //                      const std::string &file_name, const std::shared_ptr<arrow::Schema> &schema_file,
        //                      const std::shared_ptr<arrow::Schema> &schema_out, int num_rows);
        FletcherParquetReader(PlatformWrapper* platform_w, uint64_t group_length, PtoaRegs** regs, const std::string &file_name, const std::shared_ptr<arrow::Schema> &schema_file, const std::shared_ptr<arrow::Schema> &schema_out, int num_rows);
        FletcherParquetReader();
        ~FletcherParquetReader() = default;
        static double Next(PlatformWrapper* platform_w);
        static bool hasNext(PlatformWrapper* platform_w);
    };

}
