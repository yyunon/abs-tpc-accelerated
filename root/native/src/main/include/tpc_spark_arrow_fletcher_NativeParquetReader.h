/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class tpc_spark_arrow_fletcher_NativeParquetReader */

#ifndef _Included_tpc_spark_arrow_fletcher_NativeParquetReader
#define _Included_tpc_spark_arrow_fletcher_NativeParquetReader
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     tpc_spark_arrow_fletcher_NativeParquetReader
 * Method:    initNativeParquetReader
 * Signature: (Ltpc/spark/arrow/fletcher/JavaMemoryPoolServer;Ljava/lang/String;[B[BI)J
 */
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_NativeParquetReader_initNativeParquetReader
  (JNIEnv *, jobject, jobject, jstring, jbyteArray, jbyteArray, jint);

/*
 * Class:     tpc_spark_arrow_fletcher_NativeParquetReader
 * Method:    readNext
 * Signature: (J[J[J[J)Z
 */
JNIEXPORT jboolean JNICALL Java_tpc_spark_arrow_fletcher_NativeParquetReader_readNext
  (JNIEnv *, jobject, jlong, jlongArray, jlongArray, jlongArray);

/*
 * Class:     tpc_spark_arrow_fletcher_NativeParquetReader
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_NativeParquetReader_close
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
