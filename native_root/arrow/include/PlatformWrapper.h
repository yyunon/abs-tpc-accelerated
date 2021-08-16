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
        std::unique_ptr<ColumnScheduler> column_scheduler;
        //ColumnScheduler* column_scheduler;
        std::shared_ptr<fletcher::Platform> platform;
    public:
        PlatformWrapper();
        fletcher::Status Submit(std::vector<std::vector<PtoaRegs>> regs);
        double Next();
        bool hasNext(); 
        //PlatformWrapper (const PlatformWrapper&) = delete;
        //PlatformWrapper& operator= (const PlatformWrapper&) = delete;
        ~PlatformWrapper() = default;
    };

}
