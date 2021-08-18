#pragma once

#include "FileReader.h"
#include "Tpch.h"
#include "ColumnScheduler.h"
#include "Util.h"

#include <iostream>
#include <arrow/api.h>
#include <mutex>
#include <parquet/arrow/reader.h>

#include "fletcher/api.h"

namespace tpc
{
    class PlatformWrapper
    {
    private:
        std::shared_ptr<ColumnScheduler> column_scheduler;
        //ColumnScheduler* column_scheduler;
        std::shared_ptr<fletcher::Platform> platform;
    public:
        PlatformWrapper();
        fletcher::Status Submit(int num_row_groups, PtoaRegs** regs);
        double Next();
        bool hasNext(); 
        //PlatformWrapper (const PlatformWrapper&) = delete;
        //PlatformWrapper& operator= (const PlatformWrapper&) = delete;
        double duration_total;
        double duration_meta;
        double duration_compute;

        ~PlatformWrapper()
        {
          //delete column_scheduler;
        }
    };

}
