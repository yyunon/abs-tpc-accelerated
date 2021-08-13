#pragma once
// This file holds the tpch software runtime functions.
#include "Util.h"
#include "jni/Assertions.h"

namespace tpc
{
  class Tpch
  {

  public:
    std::shared_ptr<arrow::MemoryPool> _ctx; //For buffers

    Tpch() = default;

    static std::shared_ptr<Tpch> Make(uint64_t base_address, const std::shared_ptr<fletcher::Platform> &platform_g, std::shared_ptr<arrow::MemoryPool> &memory_pool)
    {
      auto tpc = std::make_shared<Tpch>();
      tpc->Init(base_address, platform_g, memory_pool);
      return tpc;
    }

    uint64_t getBaseOffset()
    {
      return base_offset;
    }

    std::shared_ptr<fletcher::Context> Context()
    {
      return context;
    }

    std::shared_ptr<fletcher::Platform> Platform()
    {
      return platform;
    }

    std::string platformName()
    {
      return platform->name();
    }

    fletcher::Status CopyBatch(const std::shared_ptr<arrow::RecordBatch> &input_batch)
    {
      ASSERT_FLETCHER_OK(context->QueueRecordBatch(input_batch));
      ASSERT_FLETCHER_OK(context->Enable());

      return fletcher::Status::OK();
    }

    fletcher::Status CopyInputOutputBatches(const std::shared_ptr<arrow::RecordBatch> &input_batch, const std::shared_ptr<arrow::RecordBatch> &output_batch)
    {
      ASSERT_FLETCHER_OK(context->QueueRecordBatch(input_batch));
      ASSERT_FLETCHER_OK(context->QueueRecordBatch(output_batch));
      ASSERT_FLETCHER_OK(context->Enable());

      return fletcher::Status::OK();
    }

    template <typename return_t>
    return_t GetReturnValueStatusReg(int k)
    {
      uint32_t rhigh;
      uint32_t rlow;
      uint64_t result;
      for (int i = 0; i < 2; i++)
      {
        uint64_t value;
        uint64_t offset = FLETCHER_REG_SCHEMA + 2 * context->num_recordbatches() + 2 * context->num_buffers() + i;
        platform->ReadMMIO64(0, offset, &value);
        value &= 0xffffffff; //the count registers are 32 bits wide, not 64
        if (i == 0)
          rhigh = (uint32_t)value;
        else
          rlow = (uint32_t)value;
      }
      result = rhigh;
      result = (result << 32) | rlow;
      double fresult = fixed_to_float(result);
      return fresult;
    }

    fletcher::Status GetReturnTable(const int &num_rows, const int &device_buffer_offset, const std::vector<uint8_t *> &buffer, const std::vector<BufferTypes> type_buffer)
    {
      int c = 0;
      for (auto b : buffer)
      {
        if (type_buffer[c] == BufferTypes::string_offset)
          platform->CopyDeviceToHost(context->device_buffer(device_buffer_offset + c).device_address, b, sizeof(int32_t) * (num_rows + 1));
        else if (type_buffer[c] == BufferTypes::string_value)
          platform->CopyDeviceToHost(context->device_buffer(device_buffer_offset + c).device_address, b, sizeof(int64_t) * 1);
        else if (type_buffer[c] == BufferTypes::value)
          platform->CopyDeviceToHost(context->device_buffer(device_buffer_offset + c).device_address, b, sizeof(int64_t) * num_rows);
        ++c;
      }
      return fletcher::Status::OK();
    }

    fletcher::Status WaitForFinish()
    {
      kernel->WaitForFinish(10);
      return fletcher::Status::OK();
    }

    std::shared_ptr<fletcher::Kernel> Kernel()
    {
      return kernel;
    }

    fletcher::Status Reset()
    {
      ASSERT_FLETCHER_OK(kernel->Reset());
      return fletcher::Status::OK();
    }

    fletcher::Status Start()
    {
      ASSERT_FLETCHER_OK(kernel->Start());
      return fletcher::Status::OK();
    }

  private:
    fletcher::Status status;

    // Only one platform is supported
    std::shared_ptr<fletcher::Platform> platform;

    //We support multiple contexts, kernels:
    //These contexts may have different base addreses.
    std::shared_ptr<fletcher::Context> context;

    std::shared_ptr<fletcher::Kernel> kernel;

    uint64_t base_offset;

    fletcher::Status Init(int base_address, const std::shared_ptr<fletcher::Platform> &platform_g, std::shared_ptr<arrow::MemoryPool> &memory_pool)
    {
      _ctx = memory_pool;
      platform = platform_g;
      //Initialize contexts and calculate base offsets.
      ASSERT_FLETCHER_OK(fletcher::Context::Make(&context, platform));
      base_offset = base_address;
      context->fletcher_snap_action_reg_offset = base_offset; //calculate_reg_base_offset(i);
      kernel = std::make_shared<fletcher::Kernel>(fletcher::Kernel(context));
      return fletcher::Status::OK();
    }
  };

  inline std::shared_ptr<arrow::RecordBatch> trivialcpuversion(const Tpch *tpch1, std::shared_ptr<arrow::Schema> in_schema)
  {
    std::vector<std::string> r = {"N", "N", "A", "R", "A", "N", "N", "R", "N", "R"};
    std::vector<std::string> l = {"F", "O", "F", "F", "F", "F", "O", "F", "O", "F"};
    //Create the schema
    std::shared_ptr<arrow::Schema> schema = std::move(in_schema);
    std::vector<std::shared_ptr<arrow::Array>> arrays;

    //Generate the values
    const int num_rows = 4;
    for (unsigned int c = 0; c < 10; ++c)
    {
      std::shared_ptr<arrow::Array> tpc_arr;
      if (c == 0)
      {
        arrow::StringBuilder stringbuilder(tpch1->_ctx.get());
        for (int i = 0; i < num_rows; ++i)
        {
          PARQUET_THROW_NOT_OK(stringbuilder.Append(r[i]));
        }
        PARQUET_THROW_NOT_OK(stringbuilder.Finish(&tpc_arr));
      }
      else if (c == 1)
      {
        arrow::StringBuilder stringbuilder(tpch1->_ctx.get());
        for (int i = 0; i < num_rows; ++i)
        {
          PARQUET_THROW_NOT_OK(stringbuilder.Append(l[i]));
        }
        PARQUET_THROW_NOT_OK(stringbuilder.Finish(&tpc_arr));
      }
      else if (c == 2 || c == 3 || c == 4 || c == 5 || c == 6 || c == 7 || c == 8)
      {
        arrow::DoubleBuilder f64builder(tpch1->_ctx.get());
        for (int i = 0; i < num_rows; i++)
        {
          double number;
          number = static_cast<double>(((long)rand() << 32) | rand());
          PARQUET_THROW_NOT_OK(f64builder.Append(number));
        }
        PARQUET_THROW_NOT_OK(f64builder.Finish(&tpc_arr));
      }
      else if (c == 9)
      {
        arrow::Int64Builder i64builder(tpch1->_ctx.get());
        for (int i = 0; i < num_rows; i++)
        {
          long number;
          number = ((long)rand() << 32) | rand();
          PARQUET_THROW_NOT_OK(i64builder.Append(number));
        }
        PARQUET_THROW_NOT_OK(i64builder.Finish(&tpc_arr));
      }
      arrays.push_back(tpc_arr);
    }

    return arrow::RecordBatch::Make(schema, num_rows, arrays);
  }

}
