/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class tpc_spark_arrow_fletcher_FletcherParquetReaderIterator */

#ifndef _Included_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator
#define _Included_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     tpc_spark_arrow_fletcher_FletcherParquetReaderIterator
 * Method:    initFletcherParquetReaderIterator
 * Signature: (JLjava/lang/String;JJ[B[BI)J
 */
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_initFletcherParquetReaderIterator
  (JNIEnv *, jobject, jlong, jstring, jlong, jlong, jbyteArray, jbyteArray, jint);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherParquetReaderIterator
 * Method:    Next
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_Next
  (JNIEnv *, jobject, jlong);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherParquetReaderIterator
 * Method:    hasNext
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_hasNext
  (JNIEnv *, jobject, jlong);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherParquetReaderIterator
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_FletcherParquetReaderIterator_close
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif