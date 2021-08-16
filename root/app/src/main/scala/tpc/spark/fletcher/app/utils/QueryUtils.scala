package tpc.spark.fletcher.app.utils

import scala.io.Source

import org.apache.spark.SparkContext
import org.apache.spark.sql._
import org.apache.spark.sql.types._
import org.apache.spark.sql.functions._

class TPCQueries(
    val spark: SparkSession,
    val query_type: String,
    val query_name: String
) {
  import spark.implicits._

  //private val file_prefix ="/home/yyunon/thesis_journals/resources/tpc-ds-sql/v2.13.0rc1/seperated_queries/"
  private val file_prefix = "/home/yyonsel/bulk/snap-build/parquet-decoder-runtime/tpc-spark-fletcher/queries/" + query_type + "/"

  def load_query() = {
    val query = Source.fromFile(file_prefix + query_name).mkString
    query
  }

}
