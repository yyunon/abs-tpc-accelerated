package tpc.spark.fletcher.app.utils

import java.io.File

object ScoptConfig {
  case class Config(
      foo: Int = -1,
      out: File = new File("."),
      optimize: Boolean = false,
      extension: String = "vanilla",
      benchmark_type: String = "tpc-h",
      file_path: String = "",
      num_rows: Int = 100,
      num_partitions: Int = 1,
      verbose: Boolean = false,
      debug: Boolean = false,
      dataset: String = "",
      mode: String = "",
      queries: Seq[Int] = Seq(),
      kwargs: Map[String, String] = Map()
  )
}
