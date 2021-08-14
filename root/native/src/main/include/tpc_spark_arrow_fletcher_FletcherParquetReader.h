/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class tpc_spark_arrow_fletcher_FletcherParquetReader */

#ifndef _Included_tpc_spark_arrow_fletcher_FletcherParquetReader
#define _Included_tpc_spark_arrow_fletcher_FletcherParquetReader
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     tpc_spark_arrow_fletcher_FletcherParquetReader
 * Method:    initFletcherParquetReader
 * Signature: (JLtpc/spark/arrow/fletcher/JavaMemoryPoolServer;Ljava/lang/String;[B[BI)J
 */
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReader_initFletcherParquetReader
  (JNIEnv *, jobject, jlong, jobject, jstring, jbyteArray, jbyteArray, jint);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherParquetReader
 * Method:    aggregate
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReader_aggregate
  (JNIEnv *, jobject, jlong);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherParquetReader
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReader_close
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif