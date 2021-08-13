package tpc.spark.arrow.fletcher

import io.netty.buffer.ArrowBuf
import org.apache.arrow.gandiva.evaluator.NativeLibraryLoader
import org.apache.arrow.gandiva.expression.ArrowTypeHelper
import org.apache.arrow.vector._
import org.apache.arrow.vector.ipc.message.ArrowFieldNode
import org.apache.arrow.vector.types.pojo.Schema
import tpc.spark.arrow.fletcher.utils.ClosableFunction

import java.io.Serializable
import java.util.logging.Logger
import scala.collection.JavaConverters._
import scala.collection.mutable

object PlatformWrapper {
  def init: Long = {
    new PlatformWrapper().ptr
  }
}
//class PlatformWrapper() extends java.io.Serializable{
class PlatformWrapper() {
//) extends ClosableFunction[Unit, Long] {

  private val log = Logger.getLogger(classOf[PlatformWrapper].getName)
  val ptr: Long = {
    NativeLibraryLoader.load()
    log.warning("initializing platform....")
    initPlatformWrapper(1)
  }

  def close(): Unit = {
    close(ptr)
  }

  @native private def initPlatformWrapper(p_type: Long): Long

  @native private def close(ptr: Long): Unit;

}
