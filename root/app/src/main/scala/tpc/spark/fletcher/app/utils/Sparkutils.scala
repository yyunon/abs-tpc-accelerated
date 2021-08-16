package tpc.spark.fletcher.app.utils

import org.apache.log4j.{Level, LogManager, Logger}
import org.apache.spark.sql.util.ArrowUtils
import org.apache.spark.sql.{SparkSession, SparkSessionExtensions}
import org.apache.spark.SparkConf
import org.apache.spark.sql.execution._
import org.apache.spark.sql.util.QueryExecutionListener
import org.apache.spark.sql.execution.{FileSourceScanExec, QueryExecution}

import tpc.spark.fletcher.extension.fletcher.parquet.{
  FletcherParquetReaderExtension,
  FletcherParquetSourceScanExec
}

import tpc.spark.fletcher.extension.fletcher.example.{
  FletcherReductionExampleExtension,
  FletcherReductionExampleExec
}
import tpc.spark.fletcher.extension.parquet.{
  ArrowParquetReaderExtension,
  ArrowParquetSourceScanExec
}

import scala.collection.mutable.ListBuffer
// This file has been migrated from Fabian Nonnmachers codebase: https://github.com/fnonnenmacher/spark-arrow-accelerated
trait InitSpark {
  final val VANILLA = "vanilla"
  final val ARROW_PARQUET = "arrow_parquet"
  final val ARROW_PARQUET_WITH_MAX_AGGREGATION = "arrow_parquet_max"
  final val FLETCHER = "fletcher"

  var metrics: ListBuffer[Seq[Long]] = ListBuffer()
  def extensionOf(s: String): Seq[SparkSessionExtensions => Unit] = s match {
    case VANILLA => Seq()
    case FLETCHER =>
      Seq(FletcherParquetReaderExtension)
    //Seq(FletcherReductionExampleExtension, ArrowParquetReaderExtension)
    case _ =>
      throw new IllegalArgumentException(s"Spark configuration $s not defined!")
  }
  def init(sparkConfigName: String) = {
    val builder = SparkSession.builder()

    extensionOf(sparkConfigName).foreach { extension =>
      builder.withExtensions(extension)
    }

    lazy val spark: SparkSession = builder
      .appName("TPC-DS-BENCHMARKING")
      .config("spark.master", "spark://qce-power9.ewi.tudelft.nl:7077")
      //.config("spark.master", "local[3]")
      .config("spark.driver.memory", "32g")
      .config("spark.sql.parquet.filterPushdown", false)
      .config("spark.sql.parquet.task.side.metadata", false)
      .config("spark.sql.inMemoryColumnarStorage.compressed", false)
      //.config("spark.sql.inMemoryColumnarStorage.batchSize", 10000000)
      .getOrCreate()

    val sc = spark.sparkContext
    val sqlContext = spark.sqlContext
    def reader =
      spark.read
        .option("header", true)
        .option("mode", "DROPMALFORMED")

    def readerWithoutHeader =
      spark.read
        .option("header", true)
        .option("inferSchema", true)
        .option("mode", "DROPMALFORMED")

    spark.sqlContext.listenerManager.register(new QueryExecutionListener {
      override def onSuccess(
          funcName: String,
          qe: QueryExecution,
          durationNs: Long
      ): Unit = {

        val MILLION: Int = 1000000
        var scanTime = 0L;
        var aggregationTime = 0L;
        var scanTimefl = 0L;
        var collectTime = 0L;
        var partitions = 0L;

        qe.executedPlan.foreach {
          case fs @ FileSourceScanExec(_, _, _, _, _, _, _) => {
            fs.metrics
              .get("scanTime")
              .foreach(m => scanTime = m.value)
            fs.metrics.get("numPartitions").foreach(m => partitions = m.value)
          }
          case ns @ ArrowParquetSourceScanExec(_, _, _) =>
            ns.metrics
              .get("scanTime")
              .foreach(m => scanTimefl += (m.value / MILLION))
          case g @ FletcherReductionExampleExec(_, _) =>
            g.metrics
              .get("aggregationTime")
              .foreach(m => aggregationTime += m.value / MILLION)
          case filter @ FilterExec(_, _) =>
            filter.metrics
              .get("collectTime")
              .foreach(m => collectTime += m.value / MILLION)
          case p @ ProjectExec(_, _) =>
            p.metrics
              .get("collectTime")
              .foreach(m => collectTime += m.value / MILLION)
          case _ =>
        }

        metrics += Seq[Long](
          scanTime,
          //collectTime,
          //scanTimefl,
          aggregationTime,
          //collectTime,
          durationNs
        )
        //println(
        //  "File scan time(Spark) is: " + scanTime + " File scan time(FPGA) is: " + scanTimefl + " Agg. time (FPGA): " + aggregationTime + " Collect time(Spark) is: " + collectTime + " Number of partitions(Spark): " + partitions + " for duration(total): " + durationNs / MILLION
        //)
      }

      override def onFailure(
          funcName: String,
          qe: QueryExecution,
          exception: Exception
      ): Unit = {}
    })
    sc.setLogLevel("WARN")
    Logger.getLogger("org").setLevel(Level.WARN)
    Logger.getLogger("akka").setLevel(Level.WARN)
    LogManager.getRootLogger.setLevel(Level.WARN)
    spark
  }
  def clearCache(spark: SparkSession): Unit = {
    spark.catalog.clearCache()
    spark.sqlContext.clearCache()
  }
}
