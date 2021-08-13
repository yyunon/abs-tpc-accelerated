package tpc.spark.arrow.fletcher

import io.netty.buffer.ArrowBuf
import org.apache.arrow.gandiva.evaluator.NativeLibraryLoader
import org.apache.arrow.gandiva.expression.ArrowTypeHelper
import org.apache.arrow.vector._
import org.apache.arrow.vector.ipc.message.ArrowFieldNode
import org.apache.arrow.vector.types.pojo.Schema

import tpc.spark.arrow.fletcher.utils.ClosableFunction
import tpc.spark.arrow.fletcher.PlatformWrapper

import org.apache.spark.sql.catalyst.InternalRow
import org.apache.spark.sql.catalyst.expressions.{
  Attribute,
  GenericInternalRow,
  UnsafeProjection
}

import java.util.logging.Logger
import scala.collection.JavaConverters._
import scala.collection.mutable

class FletcherParquetReaderIterator(
    val platform: Long,
    val fileName: String,
    val fileOffset: Long,
    val fileLength: Long,
    val inputSchema: Schema,
    val outputSchema: Schema,
    val batchSize: Int
) extends Iterator[InternalRow] {

  private val log =
    Logger.getLogger(classOf[FletcherParquetReaderIterator].getName)
  // For predicate kernel, not for projection
  //val allocator = GlobalAllocator.newChildAllocator(this.getClass)
  //val memoryPool = new JavaMemoryPoolServer(allocator)

  private var isFinished = false
  private var preLoadedVar: Option[InternalRow] = Option.empty
  private var currentVar: Option[InternalRow] = Option.empty

  private val ptr: Long = {
    NativeLibraryLoader.load()
    val inputSchemaBytes =
      ArrowTypeHelper.arrowSchemaToProtobuf(inputSchema).toByteArray
    val outputSchemaBytes =
      ArrowTypeHelper.arrowSchemaToProtobuf(outputSchema).toByteArray
    // Batch size util is not implemented.
    initFletcherParquetReaderIterator(
      platform,
      fileName,
      fileOffset,
      fileLength,
      inputSchemaBytes,
      outputSchemaBytes,
      batchSize
    )
  }

  override def hasNext: Boolean = {
    if (isFinished) return false
    if (preLoadedVar.isDefined) return true

    if (hasNext(platform) == true)
      preLoadedVar = ReadNext()

    if (preLoadedVar.isDefined) true
    else {
      isFinished = true
      close()
      false
    }
  }

  override def next(): InternalRow = {
    log.warning("Next...")
    hasNext
    val res = preLoadedVar.getOrElse(
      throw new IllegalAccessException(
        "The iterator is already closed"
      )
    )
    currentVar = preLoadedVar
    preLoadedVar = Option.empty
    res
  }

  private def toRow(res: Double): InternalRow = {
    //log.warn("RES: " + res.toString())
    val arr: Array[Any] = Array(res)
    //log.warn("RES: " + arr.toString())
    new GenericInternalRow(arr)
  }

  def ReadNext(): Option[InternalRow] = {
    val res = Next(platform)
    if (res < 0) {
      return Option.empty
    }
    Option(toRow(res))
  }

  def close(): Unit = {
    close(ptr)
    //allocator.close()
    //memoryPool.close()
  }

  @native def initFletcherParquetReaderIterator(
      platform_ptr: Long,
      fileName: String,
      fileOffset: Long,
      fileLength: Long,
      inputSchemaBytes: Array[Byte],
      outputSchemaBytes: Array[Byte],
      numRows: Int
  ): Long

  @native private def Next(
      ptr: Long
  ): Double;

  @native private def hasNext(
      ptr: Long
  ): Boolean;

  @native private def close(ptr: Long): Unit;

}
