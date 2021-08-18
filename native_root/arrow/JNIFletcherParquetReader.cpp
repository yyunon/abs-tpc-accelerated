#include "tpc_spark_arrow_fletcher_FletcherParquetReader.h"
#include "tpc_spark_arrow_fletcher_PlatformWrapper.h"
#include "tpc_spark_arrow_fletcher_FletcherParquetReaderIterator.h"

#include "FletcherParquetReader.h"
#include "PlatformWrapper.h"
#include "Assertions.h"
#include "ProtobufSchemaDeserializer.h"
#include "Converters.h"
#include "JavaMemoryPool.h"

extern "C" {
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_initFletcherParquetReaderIterator(JNIEnv *env, jobject, jlong platform_ptr,  jstring java_file_name, jlongArray extColOffset, jlongArray extColSize, jlongArray extColVal, jlongArray discColOffset, jlongArray discColSize, jlongArray discColVal, jlongArray quantColOffset, jlongArray quantColSize, jlongArray quantColVal, jlongArray shipColOffset, jlongArray shipColSize, jlongArray shipColVal, jbyteArray schema_file_jarr, jbyteArray schema_out_jarr,jint num_rows);

JNIEXPORT jdouble JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_Next(JNIEnv *env, jobject, jlong process_ptr);

JNIEXPORT jboolean JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_hasNext(JNIEnv *env, jobject, jlong process_ptr);

JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_close(JNIEnv *env, jobject, jlong process_ptr);

JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_PlatformWrapper_initPlatformWrapper(JNIEnv *env, jobject, jlong type);

JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_PlatformWrapper_close(JNIEnv *env, jobject, jlong process_ptr);
}
uint8_t *ReadFile(std::ifstream& pq_file, int offset, size_t size, size_t buffer_size)
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

//// FletcherParquetReaderIterator
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_initFletcherParquetReaderIterator(JNIEnv *env, jobject, jlong platform_ptr,  jstring java_file_name, jlongArray extColOffset, jlongArray extColSize, jlongArray extColVal, jlongArray discColOffset, jlongArray discColSize, jlongArray discColVal, jlongArray quantColOffset, jlongArray quantColSize, jlongArray quantColVal, jlongArray shipColOffset, jlongArray shipColSize, jlongArray shipColVal, jbyteArray schema_file_jarr, jbyteArray schema_out_jarr,jint num_rows)
{
      
	//std::shared_ptr<arrow::MemoryPool> pool = std::move(arrow::MemoryPool::CreateDefault());
	//
        jsize group_length = env->GetArrayLength(extColOffset);

        uint64_t *extOffset =(uint64_t*) (env->GetLongArrayElements(extColOffset, 0));
        uint64_t *extSize = (uint64_t*) (env->GetLongArrayElements(extColSize, 0));
        uint64_t *extVal = (uint64_t*) (env->GetLongArrayElements(extColVal, 0));

        uint64_t *discOffset =(uint64_t*) env->GetLongArrayElements(discColOffset, 0);
        uint64_t *discSize = (uint64_t*) env->GetLongArrayElements(discColSize, 0);
        uint64_t *discVal = (uint64_t*) env->GetLongArrayElements(discColVal, 0);

        uint64_t *quantOffset =(uint64_t*) env->GetLongArrayElements(quantColOffset, 0);
        uint64_t *quantSize = (uint64_t*) env->GetLongArrayElements(quantColSize, 0);
        uint64_t *quantVal = (uint64_t*) env->GetLongArrayElements(quantColVal, 0);

        uint64_t *shipOffset =(uint64_t*) env->GetLongArrayElements(shipColOffset, 0);
        uint64_t *shipSize = (uint64_t*) env->GetLongArrayElements(shipColSize, 0);
        uint64_t *shipVal = (uint64_t*) env->GetLongArrayElements(shipColVal, 0);

        //for(int i=0; i < group_length; ++i)
        //{
        //  std::cout << extOffset[i] << "," << extSize[i] << "," << extVal[i] << "\n";
        //  std::cout << discOffset[i] << "," << discSize[i] << "," << discVal[i] << "\n";
        //  std::cout << quantOffset[i] << "," << quantSize[i] << "," << quantVal[i] << "\n";
        //  std::cout << shipOffset[i] << "," << shipSize[i] << "," << shipVal[i] << "\n";
        //}
        PtoaRegs ** regs = new PtoaRegs*[group_length];
        for(int i = 0; i < group_length; ++i)
          regs[i] = new PtoaRegs[4];

	std::string file_name = get_java_string(env, java_file_name);
        //Read file ptoa regs
        std::ifstream pq_file(file_name);
        size_t buffer_size;
        for(int i = 0; i < group_length; ++i)
        {
          buffer_size = std::ceil((extSize[i] + 64) / 64) * 64;
          regs[i][0].max_size = buffer_size;
          regs[i][0].num_val = extVal[i];
          regs[i][0].device_parquet_address = ReadFile(pq_file, extOffset[i], extSize[i], buffer_size);

          buffer_size = std::ceil((discSize[i] + 64) / 64) * 64;
          regs[i][1].max_size = buffer_size;
          regs[i][1].num_val = discVal[i];
          regs[i][1].device_parquet_address = ReadFile(pq_file, discOffset[i], discSize[i], buffer_size);

          buffer_size = std::ceil((quantSize[i] + 64) / 64) * 64;
          regs[i][2].max_size = buffer_size;
          regs[i][2].num_val = quantVal[i];
          regs[i][2].device_parquet_address = ReadFile(pq_file, quantOffset[i], quantSize[i], buffer_size);

          buffer_size = std::ceil((shipSize[i] + 32) / 32) * 32;
          regs[i][3].max_size = buffer_size;
          regs[i][3].num_val = shipVal[i];
          regs[i][3].device_parquet_address = ReadFile(pq_file, shipOffset[i], shipSize[i], buffer_size);
        }

	jsize schema_file_len = env->GetArrayLength(schema_file_jarr);
	jbyte *schema_file_bytes = env->GetByteArrayElements(schema_file_jarr, 0);
	std::shared_ptr<arrow::Schema> schema_file = ReadSchemaFromProtobufBytes(schema_file_bytes, schema_file_len);

	jsize schema_out_len = env->GetArrayLength(schema_out_jarr);
	jbyte *schema_out_bytes = env->GetByteArrayElements(schema_out_jarr, 0);
	std::shared_ptr<arrow::Schema> schema_out = ReadSchemaFromProtobufBytes(schema_out_bytes, schema_out_len);
        
	auto process_ptr = (jlong) new tpc::FletcherParquetReader(std::move((tpc::PlatformWrapper *) platform_ptr), group_length, regs, file_name, schema_file, schema_out, (int)num_rows);
        for(int i = 0; i < group_length; ++i)
          delete[] regs[i];
        delete[] regs; 

        return process_ptr;
        //return (jlong) new tpc::FletcherParquetReader();
        //return (jlong) new tpc::FletcherParquetReader(pool, file_name, schema_file, schema_out, (int)num_rows);
}

JNIEXPORT jdouble JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_Next(JNIEnv *env, jobject, jlong process_ptr)
{

	tpc::PlatformWrapper *datasetParquetReader = std::move((tpc::PlatformWrapper *)process_ptr);
        //std::cout << "Next called \n";
	return tpc::FletcherParquetReader::Next(datasetParquetReader);
}

JNIEXPORT jboolean JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_hasNext(JNIEnv *env, jobject, jlong process_ptr)
{

	tpc::PlatformWrapper *datasetParquetReader = std::move((tpc::PlatformWrapper *)process_ptr);
        //std::cout << "Has next called \n";
	return tpc::FletcherParquetReader::hasNext(datasetParquetReader);
}


JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_close(JNIEnv *env, jobject, jlong process_ptr)
{
        //std::cout << "Closing platform \n";
	delete (tpc::FletcherParquetReader *)process_ptr;
}

//// Platform Wrapper
//
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_PlatformWrapper_initPlatformWrapper(JNIEnv *env, jobject, jlong type)
{
        //std::cout << "Initializing platform \n";
	auto ptr = new tpc::PlatformWrapper();
	return (jlong) ptr;
}

JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_PlatformWrapper_close(JNIEnv *env, jobject, jlong process_ptr)
{
        tpc::PlatformWrapper* pl = std::move((tpc::PlatformWrapper*)process_ptr);
	delete (tpc::PlatformWrapper *)process_ptr;
}
