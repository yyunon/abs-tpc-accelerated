#pragma once

#include "Util.h"

namespace tpc
{
	class FileReader
	{
	public:
		static std::shared_ptr<FileReader> Make(const std::string &file_path, const std::shared_ptr<arrow::Schema> &schema_file, bool ParseFile)
		{
			return std::make_shared<FileReader>(FileReader(file_path, schema_file, ParseFile));
		};
		FileReader(const std::string &file_path, const std::shared_ptr<arrow::Schema> &schema, bool ParseFile)
		{
			if (ParseFile == true)
				OpenAndParse(file_path, schema);
			else if (ParseFile == false)
				Open(file_path);
		};
		void OpenAndParse(const std::string &input_path, const std::shared_ptr<arrow::Schema> &schema);
		void Open(const std::string &input_path);
		uint8_t *read_file(int offset, size_t size, size_t buffer_size);
		void parseRegs(PtoaRegs *chunk, std::unique_ptr<parquet::ColumnChunkMetaData> chunk_meta);
		std::vector<std::vector<PtoaRegs>> readChunks();
		uint8_t *returnFileData();

	protected:
		std::ifstream pq_file;
		int64_t file_counter = 0;
		uint8_t *file_data;
		std::shared_ptr<arrow::Schema> input_schema;
		const parquet::SchemaDescriptor *inferred_schema;
		std::shared_ptr<parquet::ParquetFileReader> pqFileReader;
		std::shared_ptr<parquet::FileMetaData> pqFileMetadata;
		std::vector<std::shared_ptr<parquet::RowGroupMetaData>> rowGroupMetadata;
		std::vector<std::shared_ptr<parquet::ColumnChunkMetaData>> columnChunkMetadata;
		std::vector<int64_t> rowGroupByteSizes;
		bool load_file = false;
		int num_columns;
		int num_row_groups;
		size_t file_size;
	};
}
