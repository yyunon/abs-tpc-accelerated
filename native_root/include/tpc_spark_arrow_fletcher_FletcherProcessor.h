/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class tpc_spark_arrow_fletcher_FletcherProcessor */

#ifndef _Included_tpc_spark_arrow_fletcher_FletcherProcessor
#define _Included_tpc_spark_arrow_fletcher_FletcherProcessor
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     tpc_spark_arrow_fletcher_FletcherProcessor
 * Method:    initFletcherProcessor
 * Signature: (Ltpc/spark/arrow/fletcher/JavaMemoryPoolServer;[B[B)J
 */
JNIEXPORT jlong JNICALL Java_tpc_spark_arrow_fletcher_FletcherProcessor_initFletcherProcessor
  (JNIEnv *, jobject, jobject, jbyteArray, jbyteArray);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherProcessor
 * Method:    reduce
 * Signature: (JI[J[J)D
 */
JNIEXPORT jdouble JNICALL Java_tpc_spark_arrow_fletcher_FletcherProcessor_reduce
  (JNIEnv *, jobject, jlong, jint, jlongArray, jlongArray);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherProcessor
 * Method:    buildTable
 * Signature: (JI[J[J[B[B[D[D[D[D[D[D[D[J)I
 */
JNIEXPORT jint JNICALL Java_tpc_spark_arrow_fletcher_FletcherProcessor_buildTable
  (JNIEnv *, jobject, jlong, jint, jlongArray, jlongArray, jbyteArray, jbyteArray, jdoubleArray, jdoubleArray, jdoubleArray, jdoubleArray, jdoubleArray, jdoubleArray, jdoubleArray, jlongArray);

/*
 * Class:     tpc_spark_arrow_fletcher_FletcherProcessor
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_tpc_spark_arrow_fletcher_FletcherProcessor_close
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
