package tpc.spark.fletcher.app.utils

import java.lang.Boolean

object CliUtils {

  type OptionMap = Map[Symbol, Any]
  val usage =
    """
        Usage: ./gradlew run --args="[--run-benchmark | --generate-parquet] [vanilla|fletcher] query_no" 
    """
  def isNotQuery(s: String) = (
    s.toInt >= 150 && s.toInt <= 0
  )

  //https://stackoverflow.com/questions/2315912/best-way-to-parse-command-line-parameters
  def nextOption(map: OptionMap, list: List[String]): OptionMap = {
    list match {
      case Nil => map
      case "--generate-parquet" :: tail =>
        if (tail.exists(o => { o == "--run-benchmark" }) || tail.exists(
              o => { o == "--fletcher" }) || tail.exists(o => { o == "-n" })) {
          println(
            "Parquet generate and benchmark options cannot be used together."
          )
          println(usage)
          sys.exit(1)
        }
        nextOption(
          map ++ Map(Symbol("pq_gen") -> 1),
          tail
        )
      case "--num-rows" :: value :: tail =>
        nextOption(
          map ++ Map(Symbol("num_rows") -> value.toInt),
          tail
        )
      case "--run-benchmark" :: tail =>
        if (tail.exists(o => { o == "--generate-parquet" })) {
          println(
            "Parquet generate and benchmark options cannot be used together."
          )
          println(usage)
          sys.exit(1)
        }
        nextOption(
          map ++ Map(Symbol("benchmark") -> 1),
          tail
        )
      case "--vanilla" :: tail =>
        if (tail.exists(o => { o == "--fletcher" })) {
          println(
            "Parquet generate and benchmark options cannot be used together."
          )
          println(usage)
          sys.exit(1)
        }
        nextOption(
          map ++ Map(Symbol("vanilla") -> 1),
          tail
        )
      case "--fletcher" :: tail =>
        if (tail.exists(o => { o == "--vanilla" })) {
          println(
            "Parquet generate and benchmark options cannot be used together."
          )
          println(usage)
          sys.exit(1)
        }
        nextOption(
          map ++ Map(Symbol("fletcher") -> 1),
          tail
        )
      case "--optimize-fletcher" :: value :: tail =>
        nextOption(
          map ++ Map(Symbol("num_runs") -> 10),
          tail
        )
      case "-n" :: value :: tail =>
        if (isNotQuery(value)) {
          println(
            "Unknown query No"
          )
          println(usage)
          sys.exit(1)
        }
        nextOption(
          map ++ Map(Symbol("query") -> value.toInt),
          tail
        )
      case option :: tail =>
        println("Unknown option " + option)
        sys.exit(1)
    }
  }

}
