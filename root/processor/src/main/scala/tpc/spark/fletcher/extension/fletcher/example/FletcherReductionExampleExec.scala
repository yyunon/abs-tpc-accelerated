//Please check:
//package nl.tudelft.ewi.abs.nonnenmacher.fletcher.example
package tpc.spark.fletcher.extension.fletcher.example

import tpc.spark.arrow.fletcher.FletcherProcessor
import tpc.spark.arrow.fletcher.utils.AutoCloseProcessingHelper._
import tpc.spark.fletcher.extension.columnar.ArrowColumnarConverters._
import org.apache.spark.sql.vectorized.{
  ArrowColumnVector,
  ColumnarBatch,
  ColumnVector
}
import org.apache.arrow.vector.types.pojo.Schema
import org.apache.spark.sql.catalyst.plans.physical.Partitioning

import org.apache.spark.{Partition, TaskContext}
import org.apache.spark.sql.catalyst.plans.physical.{
  BroadcastMode,
  BroadcastPartitioning,
  Partitioning,
  UnknownPartitioning
}
import org.apache.spark.sql.catalyst.plans.logical.Range
import org.apache.spark.sql.catalyst.expressions.{SortOrder, Descending}
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.SparkArrowUtils
import org.apache.spark.sql.Encoders
import org.apache.spark.sql.catalyst.encoders.ExpressionEncoder
import org.apache.spark.sql.types._

import org.apache.spark.sql.catalyst.InternalRow
import org.apache.spark.sql.catalyst.expressions.{
  Attribute,
  GenericInternalRow,
  UnsafeProjection
}
import org.apache.spark.sql.catalyst.{InternalRow}
import org.apache.spark.sql.catalyst.util.DateTimeUtils.instantToMicros

import org.apache.spark.sql.execution.metric.{SQLMetric, SQLMetrics}
import org.apache.spark.sql.execution.{SparkPlan, UnaryExecNode}
import org.apache.spark.sql.types.StructType
import org.apache.spark.sql.SparkSession._
import org.apache.spark.sql.execution.LeafExecNode

import scala.collection.JavaConverters._

//import java.util.logging.Logger
case class Tpch1Row(
    l_returnflag: String,
    l_linestatus: String,
    sum_qty: Double,
    sum_base_price: Double,
    sum_disc_price: Double,
    sum_charge: Double,
    avg_qty: Double,
    avg_price: Double,
    avg_disc: Double,
    count_order: Long
)

case class FletcherReductionExampleExec(out: Seq[Attribute], child: SparkPlan)
    extends UnaryExecNode {

  //private val log = Logger.getLogger(classOf[FletcherProcessor].getName)

  private val num_rows = 4;
  private val output_iterator: Int = 0
  private def timeZoneId = conf.sessionLocalTimeZone
  //private val out_schema = this.output.schema
  private val out_schema = StructType(
    Seq(
      StructField("l_returnflag", StringType, false),
      StructField("l_linestatus", StringType, false),
      StructField("sum_qty", DoubleType, false),
      StructField("sum_base_price", DoubleType, false),
      StructField("sum_disc_price", DoubleType, false),
      StructField("sum_charge", DoubleType, false),
      StructField("avg_qty", DoubleType, false),
      StructField("avg_price", DoubleType, false),
      StructField("avg_disc", DoubleType, false),
      StructField("count_order", LongType, false)
    )
  )
  var fpga_table: Option[
    (
        Array[String],
        Array[String],
        Array[Double],
        Array[Double],
        Array[Double],
        Array[Double],
        Array[Double],
        Array[Double],
        Array[Double],
        Array[Long]
    )
  ] = Option.empty

  private val fpgaOutEncoder = Encoders.product[Tpch1Row]

  private var returnTrue: Int = 0
  private val fpgaExprEncoder =
    fpgaOutEncoder
      .asInstanceOf[ExpressionEncoder[Tpch1Row]]
      .resolveAndBind()
      .createSerializer()

  protected def doExecute(): RDD[InternalRow] = {
    val aggregationTime = longMetric("aggregationTime")

    child.executeColumnar().mapPartitionsWithIndex { (index, batches) =>
      var start: Long = 0
      start = System.nanoTime()
      val inputSchema =
        toNotNullableArrowSchema(child.schema, conf.sessionLocalTimeZone)

      val outputSchema =
        toNotNullableArrowSchema(out_schema, conf.sessionLocalTimeZone)

      val fletcherReductionProcessor =
        new FletcherProcessor(inputSchema, outputSchema)

      TaskContext.get().addTaskCompletionListener[Unit] { _ =>
        fletcherReductionProcessor.close()
      }

      var runFletcher: Boolean = false
      var batchId: Long = 0
      var outputIndex: Int = 0
      val res = batches
        .map(_.toArrow)
        .mapAndAutoClose(fletcherReductionProcessor)
      fpga_table = res.next()
      aggregationTime += System.nanoTime() - start
      new Iterator[InternalRow] {
        override def hasNext: Boolean = {
          if (outputIndex < num_rows) {
            outputIndex = outputIndex + 1
            true
          } else {
            false
          }
        }
        override def next: InternalRow = {
          toRow(
            outputIndex - 1,
            fpga_table
              .getOrElse(
                throw new IllegalAccessException("Next cant read fletcher")
              )
          )
        }
      }
    }
  }

  private def toRow(
      index: Int,
      res: (
          Array[String],
          Array[String],
          Array[Double],
          Array[Double],
          Array[Double],
          Array[Double],
          Array[Double],
          Array[Double],
          Array[Double],
          Array[Long]
      )
  ): InternalRow = {
    val arr = Tpch1Row(
      res._1(index),
      res._2(index),
      res._3(index),
      res._4(index),
      res._5(index),
      res._6(index),
      res._7(index),
      res._8(index),
      res._9(index),
      res._10(index)
    )
    fpgaExprEncoder(arr)
  }

  override def output: Seq[Attribute] = out

  override def vectorTypes: Option[Seq[String]] = Option(
    Seq.fill(out.size)(classOf[ArrowColumnVector].getName)
  )

  def toNotNullableArrowSchema(
      schema: StructType,
      timeZoneId: String
  ): Schema = {
    new Schema(schema.map { field =>
      SparkArrowUtils.toArrowField(
        field.name,
        field.dataType,
        nullable = false,
        timeZoneId
      )
    }.asJava)
  }
  def toNullableArrowSchema(schema: StructType, timeZoneId: String): Schema = {
    new Schema(schema.map { field =>
      SparkArrowUtils.toArrowField(
        field.name,
        field.dataType,
        nullable = true,
        timeZoneId
      )
    }.asJava)
  }

  override lazy val metrics: Map[String, SQLMetric] = Map(
    "aggregationTime" -> SQLMetrics.createNanoTimingMetric(
      sparkContext,
      "time aggregating in [ns]"
    )
  )
}
