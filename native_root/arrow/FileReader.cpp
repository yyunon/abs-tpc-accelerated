#include "FileReader.h"

namespace tpc 
{
  void FileReader::OpenAndParse(const std::string &input_path, const std::shared_ptr<arrow::Schema> &schema)
  {
        //input_schema = std::move(schema);
        input_schema = arrow::schema({arrow::field("l_extendedprice", arrow::float64(), false), arrow::field("l_discount", arrow::float64(), false), arrow::field("l_quantity", arrow::float64(), false), arrow::field("l_shipdate", arrow::int32(), false)});
        pqFileReader = parquet::ParquetFileReader::OpenFile(input_path, true);
        pqFileMetadata = pqFileReader->metadata();
        num_columns = pqFileMetadata->num_columns();
        num_row_groups = pqFileMetadata->num_row_groups();
        //std::cout << schema->ToString() << "\n";
        //std::cout << "num of row groups: " << num_row_groups << "\n";
        for (int i = 0; i < num_row_groups; ++i)
        {
                rowGroupMetadata.push_back(pqFileMetadata->RowGroup(i));
                inferred_schema = pqFileMetadata->RowGroup(i)->schema();
                rowGroupByteSizes.push_back(pqFileMetadata->RowGroup(i)->total_byte_size());
        }
        //std::cout << inferred_schema -> ToString() << "\n";
        Open(input_path);
        //pq_file.seekg(4, pq_file.beg); //Skip past Parquet magic number
        //Read file data
        //file_data = (uint8_t*)std::malloc(file_size);
        //posix_memalign((void **)&file_data, 4096, file_size - 4);
        //pq_file.read((char *)file_data, file_size - 4);
        //unsigned int checksum = 0;
        //for (int i = 0; i < file_size; i++)
        //{
        //	checksum += file_data[i];
        //}
        //printf("Parquet file checksum 0x%lu\n", checksum);
  }
  void FileReader::Open(const std::string &input_path)
  {
          pq_file.open(input_path);
          //Get filesize
          pq_file.seekg(0, pq_file.end);
          file_size = pq_file.tellg();
  }
  uint8_t *FileReader::read_file(int offset, size_t size, size_t buffer_size)
  {
          // Open Hexdump file
          //std::string counter = std::to_string(file_counter);
          //std::string file_name = "hexdump" + counter + ".txt";

          uint8_t *data = (uint8_t *)aligned_alloc(4096, buffer_size);
          //memset(data, 0, buffer_size);
          pq_file.seekg(offset, pq_file.beg); //Skip past Parquet magic number
          //posix_memalign((void **)&data, 4096, size);
          pq_file.read((char *)data, size);

          // Close Hexdump file
          //hexdump(file_name, data, buffer_size);
          //++file_counter;

          return data;
  }
  void FileReader::parseRegs(PtoaRegs *chunk, std::unique_ptr<parquet::ColumnChunkMetaData> chunk_meta)
  {
          size_t type_width = ((chunk_meta->type() == parquet::Type::type::DOUBLE) ? 64 : 32);
          int64_t fpo = chunk_meta->file_offset();
          size_t buffer_size = std::ceil((chunk_meta->total_uncompressed_size() + type_width) / type_width) * type_width;
          chunk->max_size = buffer_size;
          //chunk.max_size = r->ColumnChunk(i)->total_uncompressed_size();
          //chunk.max_size = r->ColumnChunk(i)->num_values() * ((r->ColumnChunk(i)->type() == parquet::Type::type::DOUBLE) ? 8 : 4);
          chunk->num_val = chunk_meta->num_values();
          auto device_addr = read_file(fpo, chunk_meta->total_uncompressed_size(), buffer_size);
          chunk->device_parquet_address = device_addr;
          printf("[METADATA_PARSER DEBUG] Metadata read file offset is : 0x%x %lu\n", fpo, fpo);
          printf("[METADATA_PARSER DEBUG] The address is : 0x%x, %lu\n", (uint64_t)chunk->device_parquet_address, (uint64_t)chunk->device_parquet_address);
          printf("[METADATA_PARSER DEBUG] The num of values are :0x%x, %lu\n ", chunk->num_val, chunk->num_val);
          printf("[METADATA_PARSER DEBUG] Uncompressed size are :0x%x, %lu\n ", chunk->max_size, chunk->max_size);
  }
  fletcher::Status FileReader::readChunks(PtoaRegs** chunks_r)
  {
          int64_t row_group_size;
          //std::vector<std::vector<PtoaRegs>> chunks_r(num_row_groups);
          //Select columns
          std::vector<std::string> selected_cols = input_schema->field_names();
          std::vector<int> selected_indexes;

          //printf("[METADATA_PARSER DEBUG] Reading started\n");
          for (auto &col : selected_cols) // Select indexes
          {
                  //std::cout << "The col: " << col << " for index " << inferred_schema->ColumnIndex(col) << "\n";
                  selected_indexes.push_back(inferred_schema->ColumnIndex(col));
          }
          int row_group_counter = 0;
          for (auto &r : rowGroupMetadata)
          {
                  printf("[METADATA_PARSER DEBUG] Reading row group no %d\n", row_group_counter);
                  int index_counter = 0;
                  for (auto &i : selected_indexes)
                  {
                          parseRegs(&chunks_r[row_group_counter][index_counter], r->ColumnChunk(i));
                          ++index_counter;
                  }
                  ++row_group_counter;
          }
          //printf("[METADATA_PARSER DEBUG] Reading done\n");
          return fletcher::Status::OK();
  }
  std::vector<std::vector<PtoaRegs>> FileReader::readChunks()
  {
          int64_t row_group_size;
          std::vector<std::vector<PtoaRegs>> chunks_r(num_row_groups);
          //Select columns
          std::vector<std::string> selected_cols = input_schema->field_names();
          std::vector<int> selected_indexes;

          for (auto &col : selected_cols) // Select indexes
          {
                  //std::cout << "The col: " << col << " for index " << inferred_schema->ColumnIndex(col) << "\n";
                  selected_indexes.push_back(inferred_schema->ColumnIndex(col));
          }
          int x = 0;
          for (auto &r : rowGroupMetadata)
          {
                  //printf("[METADATA_PARSER DEBUG] Reading row group no %d\n", x++);
                  std::vector<PtoaRegs> chunks;
                  for (auto &i : selected_indexes)
                  {
                          PtoaRegs chunk;
                          parseRegs(&chunk, r->ColumnChunk(i));
                          chunks.push_back(std::move(chunk));
                  }
                  chunks_r.push_back(chunks);
          }
          //printf("[METADATA_PARSER DEBUG] Reading done\n");
          return chunks_r;
  }
  uint8_t *FileReader::returnFileData()
  {
          return file_data;
  }
}
