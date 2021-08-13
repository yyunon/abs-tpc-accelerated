package tpc.spark.fletcher.extension.fletcher.parquet

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.Path

import org.apache.spark.sql.SparkSession

import org.apache.spark.sql.catalyst.catalog.BucketSpec
import org.apache.log4j.{Level, Logger}
//import tpc.spark.fletcher.extension.parquet.PartitionedFile
//import tpc.spark.arrow.fletcher.FletcherParquetReader
import tpc.spark.arrow.fletcher.FletcherParquetReaderIterator
import tpc.spark.arrow.fletcher.PlatformWrapper
import tpc.spark.fletcher.extension.columnar.ArrowColumnarConverters._
import tpc.spark.fletcher.extension.measuring.TimeMeasuringIterator
import org.apache.arrow.vector.types.pojo.Schema
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.SparkArrowUtils
import org.apache.spark.sql.catalyst.InternalRow
import org.apache.spark.{
  InterruptibleIterator,
  Partition,
  SparkContext,
  TaskContext
}
import org.apache.spark.rdd.{EmptyRDD, PartitionwiseSampledRDD, RDD}
import org.apache.spark.sql.catalyst.InternalRow
import org.apache.spark.sql.catalyst.expressions._
import org.apache.spark.sql.catalyst.expressions.BindReferences.bindReferences
import org.apache.spark.sql.catalyst.expressions.codegen._
import org.apache.spark.sql.catalyst.plans.physical._
import org.apache.spark.sql.execution.metric.SQLMetrics
import org.apache.spark.sql.internal.{SQLConf, StaticSQLConf}
import org.apache.spark.sql.types.{LongType, StructType}
import org.apache.spark.util.ThreadUtils
import org.apache.spark.util.random.{BernoulliCellSampler, PoissonSampler}
import org.apache.spark.sql.catalyst.expressions.aggregate._
import org.apache.spark.sql.catalyst.expressions.aggregate.AggregateExpression
import org.apache.spark.sql.catalyst.analysis.UnresolvedAttribute
import org.apache.spark.sql.catalyst.expressions.{Add, AttributeSet}
import org.apache.spark.sql.execution._
import org.apache.spark.sql.execution.datasources.PartitionedFile
import org.apache.spark.sql.execution.datasources._
import org.apache.spark.sql.execution.datasources.FilePartition
import org.apache.spark.sql.execution.{SparkPlan, LeafExecNode, UnaryExecNode}
import org.apache.spark.sql.catalyst.InternalRow
import org.apache.spark.sql.catalyst.expressions.{
  Attribute,
  GenericInternalRow,
  UnsafeProjection
}
import org.apache.spark.sql.catalyst.expressions._
import org.apache.spark.sql.execution.datasources.HadoopFsRelation
import org.apache.spark.sql.execution.metric.SQLMetrics
import org.apache.spark.sql.types._
import org.apache.spark.sql.types.StructType
import org.apache.spark.sql.vectorized.{ArrowColumnVector, ColumnarBatch}
import org.apache.spark.{Partition, TaskContext}
import org.apache.spark.sql.Encoders
import org.apache.spark.sql.catalyst.encoders.ExpressionEncoder
import org.apache.spark.util.collection.BitSet

case class Tpch6Row(
    revenue: Double
)

case class FletcherParquetSourceScanExec(
    @transient relation: HadoopFsRelation,
    outputs: Seq[Attribute],
    //namedExpr: Seq[NamedExpression],
    requiredSchema: StructType,
    partitionFilters: Seq[Expression],
    optionalBucketSet: Option[BitSet],
    dataFilters: Seq[Expression]
) extends LeafExecNode {

  //val loga = Logger.getLogger(classOf[FletcherParquetSourceScanExec])
  /////////////////////////////////////////////////////////////////////////////////
  // Serializer and helper funcs.
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  //private val tpch6schema = StructType(
  //  Seq(StructField("sum((l_extendedprice#5 * l_discount#6))", DoubleType, false))
  //)
  private val tpch6schema = StructType(
    Seq(StructField("revenue", DoubleType, false))
  )

  //private val processor : Long = PlatformWrapper.init
  private var processor: Option[Long] = Option.empty //PlatformWrapper.init
  //private val processor : Option[Long] = Option(new PlatformWrapper().ptr)

  private def toRow(res: Double): InternalRow = {
    //log.warn("RES: " + res.toString())
    val arr: Array[Any] = Array(res)
    //log.warn("RES: " + arr.toString())
    new GenericInternalRow(arr)
  }

  private def toEncodedRow(res: Double): InternalRow = {
    //val arr: Array[Any] = Array(res)
    log.warn("RES: " + res.toString())
    val arr = Tpch6Row(res)
    log.warn("RES: " + arr.toString())
    val fpgaOutEncoder = Encoders.product[Tpch6Row]
    val fpgaExprEncoder =
      fpgaOutEncoder
        .asInstanceOf[ExpressionEncoder[Tpch6Row]]
        .resolveAndBind()
        .createSerializer()
    log.warn("RES: " + fpgaExprEncoder(arr).toString())
    fpgaExprEncoder(arr)
  }
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////

  protected def doExecute(): RDD[InternalRow] = {

    //log.warn("Do Execute called...")
    processor = Option(new PlatformWrapper().ptr)
    //val scanTime = longMetric("scanTime");
    //val numOutputRows = longMetric("numOutputRows")
    var projected: Boolean = false
    inputRDD.mapPartitions { iter =>
      {
        //log.warn("Nxt...")
        iter.map { row =>
          //numOutputRows += 1
          row
        }
      }
    }
  }

  override lazy val metrics = Map(
    "scanTime" -> SQLMetrics.createNanoTimingMetric(
      sparkContext,
      "time in [ns]"
    )
  )
  override def output: Seq[Attribute] = outputs

  //override def outputExpressions: Seq[NamedExpression] = namedExpr

  override def vectorTypes: Option[Seq[String]] = Option(
    Seq.fill(outputs.size)(classOf[ArrowColumnVector].getName)
  )

  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  // Same as FileSourceScanExec
  /** Helper for computing total number and size of files in selected partitions. */
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  //private def setFilesNumAndSizeMetric(
  //    partitions: Seq[PartitionDirectory],
  //    static: Boolean
  //): Unit = {
  //  val filesNum = partitions.map(_.files.size.toLong).sum
  //  val filesSize = partitions.map(_.files.map(_.getLen).sum).sum
  //  longMetric("staticFilesNum") = filesNum
  //  longMetric("staticFilesSize") = filesSize
  //  if (relation.partitionSchemaOption.isDefined) {
  //    longMetric("numPartitions") = partitions.length
  //  }
  //}
  private def isDynamicPruningFilter(e: Expression): Boolean =
    e.find(_.isInstanceOf[PlanExpression[_]]).isDefined

  @transient private lazy val dynamicallySelectedPartitions
    : Array[PartitionDirectory] = {
    val startTime = System.nanoTime()
    val ret = relation.location.listFiles(
      partitionFilters.filterNot(isDynamicPruningFilter),
      dataFilters)
    //val ret = relation.location.inputFiles
    //setFilesNumAndSizeMetric(ret, false)
    val timeTakenMs = (System.nanoTime() - startTime) / 1000 / 1000
    //longMetric("pruningTime") = timeTakenMs
    ret
  }.toArray

  //private def hasPartitionsAvailableAtRunTime: Boolean = {
  //  partitionFilters.exists(ExecSubqueryExpression.hasSubquery)
  //}

  val fletcherReader: (PartitionedFile) => Iterator[InternalRow] =
    buildFletcherReaderWithPartitionValues(
      sparkSession = relation.sparkSession,
      dataSchema = relation.dataSchema,
      partitionSchema = tpch6schema, //relation.partitionSchema,
      requiredSchema = requiredSchema,
      options = relation.options,
      hadoopConf = relation.sparkSession.sessionState
        .newHadoopConfWithOptions(relation.options)
    )
  lazy val inputRDD: RDD[InternalRow] = {
    createReadRDD(fletcherReader, dynamicallySelectedPartitions, relation)
  }

  private def createReadRDD(
      readFile: (PartitionedFile) => Iterator[InternalRow],
      selectedPartitions: Array[PartitionDirectory],
      fsRelation: HadoopFsRelation
  ): RDD[InternalRow] = {
    val openCostInBytes =
      fsRelation.sparkSession.sessionState.conf.filesOpenCostInBytes
    val maxSplitBytes =
      FilePartition.maxSplitBytes(fsRelation.sparkSession, selectedPartitions)
    logInfo(
      s"Planning scan with bin packing, max size: $maxSplitBytes bytes, " +
        s"open cost is considered as scanning $openCostInBytes bytes."
    )

    // Filter files with bucket pruning if possible
    val bucketingEnabled =
      fsRelation.sparkSession.sessionState.conf.bucketingEnabled
    val shouldProcess: Path => Boolean = optionalBucketSet match {
      case Some(bucketSet) if bucketingEnabled =>
        // Do not prune the file if bucket file name is invalid
        filePath =>
          BucketingUtils.getBucketId(filePath.getName).forall(bucketSet.get)
      case _ =>
        _ =>
          true
    }

    val splitFiles = selectedPartitions
      .flatMap { partition =>
        partition.files.flatMap { file =>
          // getPath() is very expensive so we only want to call it once in this block:
          val filePath = file.getPath

          if (shouldProcess(filePath)) {
            val isSplitable = relation.fileFormat.isSplitable(
              relation.sparkSession,
              relation.options,
              filePath
            )
            PartitionedFileUtil.splitFiles(
              sparkSession = relation.sparkSession,
              file = file,
              filePath = filePath,
              isSplitable = false,
              maxSplitBytes = maxSplitBytes,
              partitionValues = partition.values
            )
          } else {
            Seq.empty
          }
        }
      }
      .sortBy(_.length)(implicitly[Ordering[Long]].reverse)

    val partitions =
      FilePartition.getFilePartitions(
        relation.sparkSession,
        splitFiles,
        maxSplitBytes
      )

    //new FletcherRDD(fsRelation.sparkSession, readFile, partitions)
    new FileScanRDD(fsRelation.sparkSession, readFile, partitions)
  }

  def buildFletcherReaderWithPartitionValues(
      sparkSession: SparkSession,
      dataSchema: StructType,
      partitionSchema: StructType,
      requiredSchema: StructType,
      options: Map[String, String],
      hadoopConf: Configuration
  ): PartitionedFile => Iterator[InternalRow] = {
    new (PartitionedFile => Iterator[InternalRow]) with Serializable {

      //val converter = GenerateUnsafeProjection.generate(
      //  partitionSchema.toAttributes,
      //  partitionSchema.toAttributes
      //)

      override def apply(file: PartitionedFile): Iterator[InternalRow] = {

        //val joinedRow = new JoinedRow()
        //val res = fletcherReader.run()
        //res.map { row =>
        //  converter(joinedRow(row, file.partitionValues))
        //}
        //toRow(res)
        val maxRecordsPerBatch = conf.columnBatchSize

        val inputSchema: Schema =
          SparkArrowUtils.toArrowSchema(
            partitionSchema,
            conf.sessionLocalTimeZone
          )

        val outputSchema: Schema = SparkArrowUtils.toArrowSchema(
          requiredSchema,
          conf.sessionLocalTimeZone
        )

        //log.warn("Required Schema: " + requiredSchema.toString())
        val localFileName = file.filePath.replaceFirst("file://", "")
        val fletcherReader = new FletcherParquetReaderIterator(
          processor.getOrElse(
            throw new IllegalAccessException("The Iterator is already closed")),
          localFileName,
          file.start,
          file.length,
          inputSchema,
          outputSchema,
          1000 //Not supported yet
        )

        fletcherReader
      }
    }
  }
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
}
