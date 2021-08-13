// Please check original:
//package nl.tudelft.ewi.abs.nonnenmacher
package tpc.spark.arrow.fletcher

import java.util.logging.Logger

import io.netty.buffer.ArrowBuf
import tpc.spark.arrow.fletcher.utils.ClosableFunction
import org.apache.arrow.gandiva.evaluator.NativeLibraryLoader
import org.apache.arrow.gandiva.expression.ArrowTypeHelper
import org.apache.arrow.vector._
import org.apache.arrow.vector.ipc.message.ArrowFieldNode
import org.apache.arrow.vector.ipc.message.ArrowRecordBatch
import org.apache.arrow.vector.types.pojo.Schema
import org.apache.arrow.vector.{VectorSchemaRoot, VectorUnloader}

import scala.collection.JavaConverters._
import scala.collection.mutable

class FletcherProcessor(inputSchema: Schema, outputSchema: Schema)
    extends ClosableFunction[
      VectorSchemaRoot,
      Option[
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
      ]
    ] {

  val allocator = GlobalAllocator.newChildAllocator(this.getClass)
  val memoryPool = new JavaMemoryPoolServer(allocator)

  private val log = Logger.getLogger(classOf[FletcherProcessor].getName)

  private val fieldCount = outputSchema.getFields.size();

  private val num_rows = 4;

  private var isClosed = false

  private var result: Option[
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

  private val procId: Long = {
    NativeLibraryLoader.load()
    val schemaAsBytes =
      ArrowTypeHelper.arrowSchemaToProtobuf(inputSchema).toByteArray
    val outSchemaAsBytes =
      ArrowTypeHelper.arrowSchemaToProtobuf(outputSchema).toByteArray
    initFletcherProcessor(memoryPool, schemaAsBytes, outSchemaAsBytes)
  }

  def apply(rootIn: VectorSchemaRoot): Option[
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
  ] = {

    log.warning("running instance")
    //val resultLengths: Array[Long] = Array.ofDim(fieldCount)
    //val resultNullCounts: Array[Long] = Array.ofDim(fieldCount)
    //val bufferAddresses: Array[Long] = Array.ofDim(fieldCount * 3)
    val r_c: Array[Byte] = Array.ofDim(num_rows)
    val l_c: Array[Byte] = Array.ofDim(num_rows)

    val r: Array[String] = Array.ofDim(num_rows)
    val l: Array[String] = Array.ofDim(num_rows)

    val s_q: Array[Double] = Array.ofDim(num_rows)
    val s_b: Array[Double] = Array.ofDim(num_rows)
    val s_d: Array[Double] = Array.ofDim(num_rows)
    val s_c: Array[Double] = Array.ofDim(num_rows)
    val a_q: Array[Double] = Array.ofDim(num_rows)
    val a_p: Array[Double] = Array.ofDim(num_rows)
    val a_d: Array[Double] = Array.ofDim(num_rows)
    val c_o: Array[Long] = Array.ofDim(num_rows)

    val buffersIn = BufferDescriptor(rootIn)

    buffersIn.assertAre64ByteAligned()

    val res =
      buildTable(
        procId,
        buffersIn.rowCount,
        buffersIn.addresses,
        buffersIn.sizes,
        r_c,
        l_c,
        s_q,
        s_b,
        s_d,
        s_c,
        a_q,
        a_p,
        a_d,
        c_o
      )
    var it = 0
    for (it <- 0 to num_rows - 1) {
      var a_r = Array[Byte](r_c(it))
      var a_l = Array[Byte](l_c(it))
      r(it) = new String(a_r)
      l(it) = new String(a_l)
    }

    buffersIn.close()
    if (res == 0) {
      //All entries read
      throw new IllegalArgumentException("Native returns nothing.")
    }
    Option(r, l, s_q, s_b, s_d, s_c, a_q, a_p, a_d, c_o)
  }

  @native private def initFletcherProcessor(
      jMemoryPool: JavaMemoryPoolServer,
      schemaAsBytes: Array[Byte],
      outschemaAsBytes: Array[Byte]
  ): Long

  @native private def reduce(
      procId: Long,
      rowNumbers: Int,
      inBufAddrs: Array[Long],
      inBufSized: Array[Long]
  ): Double;

  @native private def buildTable(
      procId: Long,
      rowNumbers: Int,
      inBufAddrs: Array[Long],
      inBufSized: Array[Long],
      returnflag: Array[Byte],
      linestatus: Array[Byte],
      sum_qty: Array[Double],
      sum_base_price: Array[Double],
      sum_disc_price: Array[Double],
      sum_charge: Array[Double],
      avg_qty: Array[Double],
      avg_price: Array[Double],
      avg_disc: Array[Double],
      count_order: Array[Long]
  ): Int;

  @native private def close(procId: Long): Unit;

  private case class BufferDescriptor(root: VectorSchemaRoot) {
    def close(): Any = recordBatch.close();

    lazy val recordBatch: ArrowRecordBatch = new VectorUnloader(
      root
    ).getRecordBatch
    lazy val buffers: Seq[ArrowBuf] = recordBatch.getBuffers.asScala
    lazy val rowCount: Int = root.getRowCount
    lazy val addresses: Array[Long] = buffers.map(_.memoryAddress()).toArray
    lazy val sizes: Array[Long] = buffers.map(_.readableBytes()).toArray

    def assertAre64ByteAligned(): Unit = {

      val sb = new StringBuilder()
      var isAligned = true

      buffers.foreach { b =>
        sb.append(
          s"Addr: ${b.memoryAddress().toHexString} % 64 = ${b.memoryAddress() % 64} "
        )
        sb.append(s"Capacity: ${b.capacity()} % 64 = ${b.capacity() % 64} ")
        sb.append("\n")

        isAligned &= b.memoryAddress() % 64 == 0
        isAligned &= b.capacity() % 64 == 0
      }

      if (!isAligned) {
        log.warning("Buffers are not aligned. \n" + sb.toString())
      }
    }
  }

  def printL(inp: Array[Long], name: String): Unit = {
    val sb = new StringBuilder()

    sb.append("\n" + name)
    inp.foreach { i =>
      sb.append(s"\n${i}")
    }
    sb.append("\n")
    log.warning(sb.toString())

  }

  def close(): Unit = {
    if (!isClosed) {
      isClosed = true
      close(procId)
      allocator.close()
    }
  }
}
