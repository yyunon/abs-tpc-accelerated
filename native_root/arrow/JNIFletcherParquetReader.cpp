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
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_initFletcherParquetReaderIterator(JNIEnv *env, jobject, jlong platform_ptr,  jstring java_file_name, jlong file_offset, jlong file_length, jbyteArray schema_file_jarr, jbyteArray schema_out_jarr,jint num_rows);

JNIEXPORT jdouble JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_Next(JNIEnv *env, jobject, jlong process_ptr);

JNIEXPORT jboolean JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_hasNext(JNIEnv *env, jobject, jlong process_ptr);

JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_close(JNIEnv *env, jobject, jlong process_ptr);

JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_PlatformWrapper_initPlatformWrapper(JNIEnv *env, jobject, jlong type);

JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_PlatformWrapper_close(JNIEnv *env, jobject, jlong process_ptr);
}
//// FletcherParquetReaderIterator
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_initFletcherParquetReaderIterator(JNIEnv *env, jobject, jlong platform_ptr,  jstring java_file_name, jlong file_offset, jlong file_length, jbyteArray schema_file_jarr,
																																																			jbyteArray schema_out_jarr,
																																																			jint num_rows)
{
      
	//std::shared_ptr<arrow::MemoryPool> pool = std::move(arrow::MemoryPool::CreateDefault());

	std::string file_name = get_java_string(env, java_file_name);

	jsize schema_file_len = env->GetArrayLength(schema_file_jarr);
	jbyte *schema_file_bytes = env->GetByteArrayElements(schema_file_jarr, 0);
	std::shared_ptr<arrow::Schema> schema_file = ReadSchemaFromProtobufBytes(schema_file_bytes, schema_file_len);

	jsize schema_out_len = env->GetArrayLength(schema_out_jarr);
	jbyte *schema_out_bytes = env->GetByteArrayElements(schema_out_jarr, 0);
	std::shared_ptr<arrow::Schema> schema_out = ReadSchemaFromProtobufBytes(schema_out_bytes, schema_out_len);
        
        //std::cout << "File path: " << file_name << " offset " << file_offset << " size " << file_length << "\n";
        
	auto process_ptr = (jlong) new tpc::FletcherParquetReader(std::move((tpc::PlatformWrapper *) platform_ptr), file_name, schema_file, schema_out, (int)num_rows);
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
        std::cout << "Initializing platform \n";
	auto ptr = new tpc::PlatformWrapper();
	return (jlong) ptr;
}

JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_PlatformWrapper_close(JNIEnv *env, jobject, jlong process_ptr)
{
        //std::cout << "Closing platform \n";
	delete (tpc::PlatformWrapper *)process_ptr;
}
