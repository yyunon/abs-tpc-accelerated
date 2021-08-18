#pragma once

#include <chrono>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <future>
// Apache Arrow
#include "Assertions.h"
#include "Macro.h"
#include <fletcher/api.h>
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/api/reader.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>

struct PtoaRegs
{
	uint64_t num_val;
	uint64_t max_size;
	uint8_t *device_parquet_address;
};

namespace tpc
{
        class timer {
          public:
              std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
              timer() : lastTime(std::chrono::high_resolution_clock::now()) {}
              inline double elapsed() {
                  std::chrono::time_point<std::chrono::high_resolution_clock> thisTime=std::chrono::high_resolution_clock::now();
                  double deltaTime = std::chrono::duration<double>(thisTime-lastTime).count();
                  lastTime = thisTime;
                  return deltaTime;
              }
        };
        

	//void setPtoaArguments(uint64_t col_base_offset, std::shared_ptr<fletcher::Platform> platform, uint32_t num_val,
	//                      uint64_t max_size, da_t device_parquet_address)
	inline void setPtoaArguments(uint64_t col_base_offset, std::shared_ptr<fletcher::Platform> platform, PtoaRegs p)
	{
		dau_t mmio64_writer;

		platform->WriteMMIO(col_base_offset, REG_BASE + 0, p.num_val);

		mmio64_writer.full = (da_t)p.device_parquet_address;
		platform->WriteMMIO(col_base_offset, REG_BASE + 1, mmio64_writer.lo);
		platform->WriteMMIO(col_base_offset, REG_BASE + 2, mmio64_writer.hi);

		mmio64_writer.full = p.max_size;
		platform->WriteMMIO(col_base_offset, REG_BASE + 3, mmio64_writer.lo);
		platform->WriteMMIO(col_base_offset, REG_BASE + 4, mmio64_writer.hi);

		return;
	}
	//Use standard Arrow library functions to read Arrow array from Parquet file
	//Only works for Parquet version 1 style files.
	inline std::shared_ptr<arrow::ChunkedArray> readArray(std::string hw_input_file_path)
	{
		std::shared_ptr<arrow::io::ReadableFile> infile;
		arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> result = arrow::io::ReadableFile::Open(hw_input_file_path);
		if (result.ok())
		{
			infile = result.ValueOrDie();
		}
		else
		{
			printf("Error opening Parquet file: code %d, error message: %s\n",
						 result.status().code(), result.status().message().c_str());
			exit(-1);
		}

		std::unique_ptr<parquet::arrow::FileReader> reader;
		parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader);

		std::shared_ptr<arrow::ChunkedArray> array;
		reader->ReadColumn(0, &array);

		return array;
	}

	inline void hexdump(std::string file_name, void *pAddressIn, long lSize)
	{
		FILE *file = fopen(file_name.c_str(), "w+");
		char szBuf[100];
		long lIndent = 1;
		long lOutLen, lIndex, lIndex2, lOutLen2;
		long lRelPos;
		struct
		{
			char *pData;
			unsigned long lSize;
		} buf;
		unsigned char *pTmp, ucTmp;
		unsigned char *pAddress = (unsigned char *)pAddressIn;

		buf.pData = (char *)pAddress;
		buf.lSize = lSize;

		while (buf.lSize > 0)
		{
			pTmp = (unsigned char *)buf.pData;
			lOutLen = (int)buf.lSize;
			if (lOutLen > 16)
				lOutLen = 16;

			// create a 64-character formatted output line:
			sprintf(szBuf, " >                            "
										 "                      "
										 "    %08lX",
							pTmp - pAddress);
			lOutLen2 = lOutLen;

			for (lIndex = 1 + lIndent, lIndex2 = 53 - 15 + lIndent, lRelPos = 0;
					 lOutLen2;
					 lOutLen2--, lIndex += 2, lIndex2++)
			{
				ucTmp = *pTmp++;

				sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
				if (!isprint(ucTmp))
					ucTmp = '.'; // nonprintable char
				szBuf[lIndex2] = ucTmp;

				if (!(++lRelPos & 3)) // extra blank after 4 bytes
				{
					lIndex++;
					szBuf[lIndex + 2] = ' ';
				}
			}

			if (!(lRelPos & 3))
				lIndex--;

			szBuf[lIndex] = '<';
			szBuf[lIndex + 1] = ' ';

			fprintf(file, "%s\n", szBuf);

			buf.pData += lOutLen;
			buf.lSize -= lOutLen;
		}
		fclose(file);
	}

	inline std::shared_ptr<arrow::RecordBatch> prepareFinalRecordBatch(int32_t num_val)
	{
		std::vector<std::shared_ptr<arrow::Buffer>> buffer_vector(4);
		int i = 0;
		for (auto &values : buffer_vector)
		{
			uint64_t *values_ptr = nullptr;
			size_t values_size;
			if (i == 3)
				values_size = sizeof(int32_t) * num_val;
			else
				values_size = sizeof(int64_t) * num_val;
			int retval = posix_memalign(reinterpret_cast<void **>(&values_ptr), 4096, values_size);
			if (retval)
			{
				//printf("Error %d allocating aligned memory for output\n", retval);
			}
			memset(values_ptr, 0, values_size);
			values = arrow::MutableBuffer::Wrap(values_ptr, num_val); //Wrap uses the sizeof of the template of the pointer
			if (values->is_mutable())
			{
				//printf("Buffer is mutable\n");
			}
			else
			{
				//printf("Buffer is NOT mutable\n");
			}
			//printf("allocated memory address 0x%p, size %lu. arrow buffer mutable data address 0x%p, size %lu\n", values_ptr, values_size, values->mutable_data(), values->size());
			++i;
		}

		std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("l_extendedprice", arrow::float64(), false), arrow::field("l_discount", arrow::float64(), false), arrow::field("l_quantity", arrow::float64(), false), arrow::field("l_shipdate", arrow::date32(), false)});

		auto l_extendedprice_arr = std::make_shared<arrow::DoubleArray>(num_val, buffer_vector[0]);
		auto l_discount_arr = std::make_shared<arrow::DoubleArray>(num_val, buffer_vector[1]);
		auto l_quantity_arr = std::make_shared<arrow::DoubleArray>(num_val, buffer_vector[2]);
		auto l_shipdate_arr = std::make_shared<arrow::Date32Array>(num_val, buffer_vector[3]);

		// Final recordbatch
		//std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_orderkey_arr, l_partkey_arr, l_suppkey_arr, l_linenumber_arr, l_quantity_arr, l_extendedprice_arr, l_discount_arr, l_tax_arr, l_returnflag_arr, l_linestatus_arr, l_shipdate_arr, l_commitdate_arr, l_receiptdate_arr, l_shipinstruct_arr, l_shipmode_arr, l_comment_arr};
		std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_extendedprice_arr, l_discount_arr, l_quantity_arr, l_shipdate_arr};
		auto recordbatch = arrow::RecordBatch::Make(schema, num_val, output_arrs);
		return recordbatch;
	}

	inline std::shared_ptr<arrow::RecordBatch> prepareRecordBatchC1(int32_t num_val)
	{
		std::shared_ptr<arrow::Buffer> values;
		uint64_t *values_ptr = nullptr;
		const size_t values_size = sizeof(int64_t) * num_val;
		int retval = posix_memalign(reinterpret_cast<void **>(&values_ptr), 4096, values_size);
		if (retval)
		{
			//printf("Error %d allocating aligned memory for output\n", retval);
		}
		memset(values_ptr, 0, values_size);
		values = arrow::MutableBuffer::Wrap(values_ptr, num_val); //Wrap uses the sizeof of the template of the pointer
		if (values->is_mutable())
		{
			//printf("Buffer is mutable\n");
		}
		else
		{
			//printf("Buffer is NOT mutable\n");
		}
		//printf("allocated memory address 0x%p, size %lu. arrow buffer mutable data address 0x%p, size %lu\n", values_ptr, values_size, values->mutable_data(), values->size());

		std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("l_quantity", arrow::float64(), false)});

		auto l_quantity_arr = std::make_shared<arrow::DoubleArray>(num_val, values);

		// Final recordbatch
		//std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_orderkey_arr, l_partkey_arr, l_suppkey_arr, l_linenumber_arr, l_quantity_arr, l_extendedprice_arr, l_discount_arr, l_tax_arr, l_returnflag_arr, l_linestatus_arr, l_shipdate_arr, l_commitdate_arr, l_receiptdate_arr, l_shipinstruct_arr, l_shipmode_arr, l_comment_arr};
		std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_quantity_arr};
		auto recordbatch = arrow::RecordBatch::Make(schema, num_val, output_arrs);
		return recordbatch;
	}

	inline std::shared_ptr<arrow::RecordBatch> prepareRecordBatchC2(int32_t num_val)
	{
		std::shared_ptr<arrow::Buffer> values;
		uint64_t *values_ptr = nullptr;
		const size_t values_size = sizeof(int64_t) * num_val;
		int retval = posix_memalign(reinterpret_cast<void **>(&values_ptr), 4096, values_size);
		if (retval)
		{
			//printf("Error %d allocating aligned memory for output\n", retval);
		}
		memset(values_ptr, 0, values_size);
		values = arrow::MutableBuffer::Wrap(values_ptr, num_val); //Wrap uses the sizeof of the template of the pointer
		if (values->is_mutable())
		{
			//printf("Buffer is mutable\n");
		}
		else
		{
			//printf("Buffer is NOT mutable\n");
		}
		//printf("allocated memory address 0x%p, size %lu. arrow buffer mutable data address 0x%p, size %lu\n", values_ptr, values_size, values->mutable_data(), values->size());

		std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("l_extendedprice", arrow::float64(), false)});

		auto l_extendedprice_arr = std::make_shared<arrow::DoubleArray>(num_val, values);

		// Final recordbatch
		//std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_orderkey_arr, l_partkey_arr, l_suppkey_arr, l_linenumber_arr, l_quantity_arr, l_extendedprice_arr, l_discount_arr, l_tax_arr, l_returnflag_arr, l_linestatus_arr, l_shipdate_arr, l_commitdate_arr, l_receiptdate_arr, l_shipinstruct_arr, l_shipmode_arr, l_comment_arr};
		std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_extendedprice_arr};
		auto recordbatch = arrow::RecordBatch::Make(schema, num_val, output_arrs);
		return recordbatch;
	}

	inline std::shared_ptr<arrow::RecordBatch> prepareRecordBatchC3(int32_t num_val)
	{
		std::shared_ptr<arrow::Buffer> values;
		uint64_t *values_ptr = nullptr;
		const size_t values_size = sizeof(int64_t) * num_val;
		int retval = posix_memalign(reinterpret_cast<void **>(&values_ptr), 4096, values_size);
		if (retval)
		{
			//printf("Error %d allocating aligned memory for output\n", retval);
		}
		memset(values_ptr, 0, values_size);
		values = arrow::MutableBuffer::Wrap(values_ptr, num_val); //Wrap uses the sizeof of the template of the pointer
		if (values->is_mutable())
		{
			//printf("Buffer is mutable\n");
		}
		else
		{
			//printf("Buffer is NOT mutable\n");
		}
		//printf("allocated memory address 0x%p, size %d. arrow buffer mutable data address 0x%p, size %d\n", values_ptr, values_size, values->mutable_data(), values->size());

		std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("l_discount", arrow::float64(), false)});

		auto l_discount_arr = std::make_shared<arrow::DoubleArray>(num_val, values);

		// Final recordbatch
		//std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_orderkey_arr, l_partkey_arr, l_suppkey_arr, l_linenumber_arr, l_quantity_arr, l_extendedprice_arr, l_discount_arr, l_tax_arr, l_returnflag_arr, l_linestatus_arr, l_shipdate_arr, l_commitdate_arr, l_receiptdate_arr, l_shipinstruct_arr, l_shipmode_arr, l_comment_arr};
		std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_discount_arr};
		auto recordbatch = arrow::RecordBatch::Make(schema, num_val, output_arrs);
		return recordbatch;
	}

	inline std::shared_ptr<arrow::RecordBatch> prepareRecordBatchC4(int32_t num_val)
	{
		std::shared_ptr<arrow::Buffer> values;
		uint64_t *values_ptr = nullptr;
		const size_t values_size = sizeof(int32_t) * num_val;
		int retval = posix_memalign(reinterpret_cast<void **>(&values_ptr), 4096, values_size);
		if (retval)
		{
			//printf("Error %d allocating aligned memory for output\n", retval);
		}
		memset(values_ptr, 0, values_size);
		values = arrow::MutableBuffer::Wrap(values_ptr, num_val); //Wrap uses the sizeof of the template of the pointer
		if (values->is_mutable())
		{
			//printf("Buffer is mutable\n");
		}
		else
		{
			//printf("Buffer is NOT mutable\n");
		}
		//printf("allocated memory address 0x%p, size %d. arrow buffer mutable data address 0x%p, size %d\n", values_ptr, values_size, values->mutable_data(), values->size());

		std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("l_shipdate", arrow::date32(), false)});

		auto l_shipdate_arr = std::make_shared<arrow::Date32Array>(num_val, values);

		// Final recordbatch
		//std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_orderkey_arr, l_partkey_arr, l_suppkey_arr, l_linenumber_arr, l_quantity_arr, l_extendedprice_arr, l_discount_arr, l_tax_arr, l_returnflag_arr, l_linestatus_arr, l_shipdate_arr, l_commitdate_arr, l_receiptdate_arr, l_shipinstruct_arr, l_shipmode_arr, l_comment_arr};
		std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_shipdate_arr};
		auto recordbatch = arrow::RecordBatch::Make(schema, num_val, output_arrs);
		return recordbatch;
	}
	inline fletcher::Status WaitForFinish(std::shared_ptr<fletcher::Platform> platform, uint64_t offset, unsigned int poll_interval_usec)
	{
		//FLETCHER_LOG(DEBUG, "Polling kernel for completion.");
		uint32_t status = 0;
		uint32_t done_status = 1ul << 0x2u;
		uint32_t done_status_mask = 1ul << 0x2u;
		if (poll_interval_usec == 0)
		{
			do
			{
				platform->ReadMMIO(offset, 0x01, &status);
			} while ((status & done_status_mask) != done_status);
		}
		else
		{
			do
			{
				usleep(poll_interval_usec);
				platform->ReadMMIO(offset, 0x01, &status);
			} while ((status & done_status_mask) != done_status);
		}
		//FLETCHER_LOG(DEBUG, "Kernel status done bit asserted.");
		return fletcher::Status::OK();
	}
	//inline fletcher::Status WriteMetaData(std::shared_ptr<fletcher::Platform> platform_t)
	//{
	//	using namespace fletcher;
	//	Status status;
	//	FLETCHER_LOG(DEBUG, "Writing context metadata to kernel.");

	//	// Set the starting offset to the first schema-derived register index.
	//	uint64_t offset = FLETCHER_REG_SCHEMA;

	//	auto context_g = std::atomic_load(&context_t);
	//	// Get the platform pointer.
	//	auto platform = std::atomic_load(&platform_t);

	//	// Write RecordBatch ranges.
	//	for (size_t i = 0; i < context_g->num_recordbatches(); i++)
	//	{
	//		auto rb = context_g->recordbatch(i);
	//		status = platform->WriteMMIO(context_g->fletcher_snap_action_reg_offset, offset, 0); // First index
	//		if (!status.ok())
	//			return status;
	//		offset++;
	//		status = platform->WriteMMIO(context_g->fletcher_snap_action_reg_offset, offset, rb->num_rows()); // Last index (exclusive)
	//		if (!status.ok())
	//			return status;
	//		offset++;
	//	}

	//	// Write buffer addresses
	//	for (size_t i = 0; i < context_g->num_buffers(); i++)
	//	{
	//		// Get the device address
	//		auto device_buf = context_g->device_buffer(i);
	//		dau_t address;
	//		address.full = device_buf.device_address;
	//		// Write the address
	//		platform->WriteMMIO(context_g->fletcher_snap_action_reg_offset, offset, address.lo);
	//		if (!status.ok())
	//			return status;
	//		offset++;
	//		platform->WriteMMIO(context_g->fletcher_snap_action_reg_offset, offset, address.hi);
	//		if (!status.ok())
	//			return status;
	//		offset++;
	//	}
	//	return Status::OK();
	//}

	//Helper functions, for type conversion:
	constexpr uint64_t calculate_reg_base_offset(int i)
	{
		return ((1 << 16) + i * (1 << 12));
	}

	//FPGA runs fixed point arithmetic
	inline double fixed_to_float(int64_t input)
	{
		return ((double)input / (double)(1 << 18));
	}

	template <class BaseType>
	inline BaseType float_to_fixed(double input)
	{
		return (BaseType)((1 << 18) * input);
	}

	//Buffer Types are for data copy
	enum BufferTypes
	{
		string_offset = 0,
		string_value,
		value
	};

	inline double ReadResultFloat(std::shared_ptr<fletcher::Platform> platform, uint64_t base_offset)
	{
		uint32_t rlow, rhigh;
		platform->ReadMMIO(base_offset, 0xF, &rlow);
		platform->ReadMMIO(base_offset, 0xE, &rhigh);
		uint64_t result = rhigh;
		result = (result << 32) | rlow;
		return fixed_to_float(result);
	}

	inline double tpch6cpuversion(const std::shared_ptr<arrow::RecordBatch> &record_batch)
	{
		auto quantity = std::static_pointer_cast<arrow::DoubleArray>(record_batch->column(0));
		auto ext = std::static_pointer_cast<arrow::DoubleArray>(record_batch->column(1));
		auto discount = std::static_pointer_cast<arrow::DoubleArray>(record_batch->column(2));
		auto shipdate = std::static_pointer_cast<arrow::Int32Array>(record_batch->column(3));

		const int32_t *ship_date_raw = shipdate->raw_values();
		const double *discount_raw = discount->raw_values();
		const double *quantity_raw = quantity->raw_values();
		const double *ext_raw = ext->raw_values();

		//std::cout << "Constants are: " << quantity_constant << "," << discount_down_constant << "," << discount_up_constant << "\n";
		double sum = 0;
		//std::cout << "Number of rows are: " << record_batch->num_rows() << "\n";
		for (int i = 0; i < record_batch->num_rows(); ++i)
		{
			//std::cout << quantity_raw[i] << "," << ext_raw[i] << "," << discount_raw[i] << "," << ship_date_raw[i] << "\n";
			if (quantity_raw[i] < 24.0 && ship_date_raw[i] < 9131 && ship_date_raw[i] >= 8766 && discount_raw[i] <= 0.061 && discount_raw[i] >= 0.059)
				sum += ext_raw[i] * discount_raw[i];
		}
		std::cout << sum << "\n";
		return sum;
	}

	// Prepare recordbatch to hold the output data
	inline std::shared_ptr<arrow::RecordBatch> PrepareOutputRecordBatchQuery1(int32_t num_strings, int32_t num_chars, int32_t num_rows)
	{
		std::shared_ptr<arrow::Buffer> l_returnflag_offsets;
		std::shared_ptr<arrow::Buffer> l_returnflag_values;
		std::shared_ptr<arrow::Buffer> sum_qty, sum_base_price, sum_disc_price, sum_charge, avg_qty, avg_price, avg_disc, count_order;

		l_returnflag_offsets = arrow::AllocateBuffer(sizeof(int32_t) * (num_strings + 1)).ValueOrDie();
		l_returnflag_values = arrow::AllocateBuffer(num_chars).ValueOrDie();

		std::shared_ptr<arrow::Buffer> l_linestatus_offsets;
		std::shared_ptr<arrow::Buffer> l_linestatus_values;
		l_linestatus_offsets = arrow::AllocateBuffer(sizeof(int32_t) * (num_strings + 1)).ValueOrDie();
		l_linestatus_values = arrow::AllocateBuffer(num_chars).ValueOrDie();

		sum_qty = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();
		sum_base_price = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();
		sum_disc_price = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();
		sum_charge = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();
		avg_qty = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();
		avg_disc = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();
		avg_price = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();
		count_order = arrow::AllocateBuffer(sizeof(int64_t) * num_rows).ValueOrDie();

		std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("l_returnflag", arrow::utf8(), false),
																													 arrow::field("l_linestatus", arrow::utf8(), false),
																													 arrow::field("sum_qty", arrow::int64(), false),
																													 arrow::field("sum_base_price", arrow::int64(), false),
																													 arrow::field("sum_disc_price", arrow::int64(), false),
																													 arrow::field("sum_charge", arrow::int64(), false),
																													 arrow::field("avg_qty", arrow::int64(), false),
																													 arrow::field("avg_price", arrow::int64(), false),
																													 arrow::field("avg_disc", arrow::int64(), false),
																													 arrow::field("count_order", arrow::int64(), false)});
		auto l_returnflag_arr = std::make_shared<arrow::StringArray>(num_rows, l_returnflag_offsets, l_returnflag_values);
		auto l_linestatus_arr = std::make_shared<arrow::StringArray>(num_rows, l_linestatus_offsets, l_linestatus_values);
		auto sum_qty_arr = std::make_shared<arrow::Int64Array>(num_rows, sum_qty);
		auto sum_base_price_arr = std::make_shared<arrow::Int64Array>(num_rows, sum_base_price);
		auto sum_disc_price_arr = std::make_shared<arrow::Int64Array>(num_rows, sum_disc_price);
		auto sum_charge_arr = std::make_shared<arrow::Int64Array>(num_rows, sum_charge);
		auto avg_qty_arr = std::make_shared<arrow::Int64Array>(num_rows, avg_qty);
		auto avg_disc_arr = std::make_shared<arrow::Int64Array>(num_rows, avg_disc);
		auto avg_price_arr = std::make_shared<arrow::Int64Array>(num_rows, avg_price);
		auto count_order_arr = std::make_shared<arrow::Int64Array>(num_rows, count_order);
		// Final recordbatch
		std::vector<std::shared_ptr<arrow::Array>> output_arrs = {l_returnflag_arr, l_linestatus_arr, sum_qty_arr, sum_base_price_arr, sum_disc_price_arr, sum_charge_arr, avg_qty_arr, avg_price_arr, avg_disc_arr, count_order_arr};
		auto recordbatch = arrow::RecordBatch::Make(schema, num_rows, output_arrs);
		return recordbatch;
	}

}
