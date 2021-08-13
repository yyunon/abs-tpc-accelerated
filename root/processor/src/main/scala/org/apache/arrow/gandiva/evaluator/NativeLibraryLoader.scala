package org.apache.arrow.gandiva.evaluator

object NativeLibraryLoader {

  def load(): Unit = {
    _load
  }

  private lazy val _load: Boolean = {
    JniLoader
      .getInstance() //needs to load gandiva libraries first to avoid conflicts (only package visible)
    System.setProperty(
      "java.library.path",
      "/shares/bulk/yyonsel/snap-build/parquet-decoder-runtime/tpc-spark-fletcher/arrow-processor-native/build/lib/main/debug:/home/yyonsel/bulk/project/local/lib:/home/yyonsel/bulk/project/local/lib64:/usr/local/lib64:/usr/local/lib"
    )
    //System.setProperty("java.library.path", "/home/yyonsel/bulk/project/local/lib:/home/yyonsel/bulk/project/local/lib64:/usr/local/lib64:/usr/local/lib")
    val libpath = System.getProperty("java.library.path")
    println(libpath)
    System.loadLibrary("protobuf")
    System.loadLibrary("arrow")
    System.loadLibrary("parquet")
    System.loadLibrary("arrow_dataset")
    System.loadLibrary("ocxl")
    System.loadLibrary("fletcher_snap")
    System.loadLibrary("fletcher")
    System.loadLibrary("arrow-processor-native")
    true
  }
}
