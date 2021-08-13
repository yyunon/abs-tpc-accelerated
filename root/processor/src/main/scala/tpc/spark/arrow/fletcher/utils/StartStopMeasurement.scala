package tpc.spark.fletcher.utils

trait StartStopMeasurement {

  private var current_start: Long = 0

  def start(): Unit = {
    current_start = System.nanoTime()
  }

  def stop(): Long = {
    val end = System.nanoTime()
    end - current_start
  }
}
