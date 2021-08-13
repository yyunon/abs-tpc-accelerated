package tpc.spark.arrow.fletcher

import io.netty.buffer.ArrowBuf
import org.apache.arrow.gandiva.evaluator.NativeLibraryLoader
import org.apache.arrow.gandiva.expression.ArrowTypeHelper
import org.apache.arrow.vector._
import org.apache.arrow.vector.ipc.message.ArrowFieldNode
import org.apache.arrow.vector.types.pojo.Schema
import tpc.spark.arrow.fletcher.utils.ClosableFunction
import tpc.spark.arrow.fletcher.PlatformWrapper

import java.util.logging.Logger
import scala.collection.JavaConverters._
import scala.collection.mutable

class FletcherParquetReader(
    val platform: Long,
    val fileName: String,
    val inputSchema: Schema,
    val outputSchema: Schema,
    val batchSize: Int
) {
//) extends ClosableFunction[Unit, Double] {

  private val log = Logger.getLogger(classOf[FletcherParquetReader].getName)
  // For predicate kernel, not for projection
  val allocator = GlobalAllocator.newChildAllocator(this.getClass)
  val memoryPool = new JavaMemoryPoolServer(allocator)

  private val ptr: Long = {
    NativeLibraryLoader.load()
    val inputSchemaBytes =
      ArrowTypeHelper.arrowSchemaToProtobuf(inputSchema).toByteArray
    val outputSchemaBytes =
      ArrowTypeHelper.arrowSchemaToProtobuf(outputSchema).toByteArray
    // Batch size util is not implemented
    initFletcherParquetReader(
      platform,
      memoryPool,
      fileName,
      inputSchemaBytes,
      outputSchemaBytes,
      batchSize
    )
  }

  def run(): Double = {
    val res = aggregate(ptr)
    //log.warning("Result is: " + res.toString())
    res
  }

  private val fieldCount = outputSchema.getFields.size();

  def close(): Unit = {
    close(ptr)
    allocator.close()
    memoryPool.close()
  }

  @native def initFletcherParquetReader(
      platform_ptr: Long,
      jMemoryPool: JavaMemoryPoolServer,
      fileName: String,
      inputSchemaBytes: Array[Byte],
      outputSchemaBytes: Array[Byte],
      numRows: Int
  ): Long

  @native private def aggregate(
      ptr: Long
  ): Double;

  @native private def close(ptr: Long): Unit;

}
