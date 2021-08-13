package tpc.spark.fletcher.extension.fletcher.parquet

import org.apache.spark.sql.SparkSessionExtensions
import org.apache.spark.sql.catalyst.rules.Rule
import org.apache.spark.sql.execution._
import org.apache.spark.sql.execution.aggregate._
import org.apache.spark.sql.execution.exchange._
import org.apache.spark.sql.execution.columnar._
import org.apache.spark.sql.execution.datasources.parquet.ParquetFileFormat

object FletcherParquetReaderExtension extends (SparkSessionExtensions => Unit) {
  override def apply(e: SparkSessionExtensions) {
    e.injectColumnar(_ => FletcherParquetReaderStrategy)
  }

  private object FletcherParquetReaderStrategy extends ColumnarRule {
    override def preColumnarTransitions: Rule[SparkPlan] = { p: SparkPlan =>
      p.transformDown {
        case f @ HashAggregateExec(_,
                                   _,
                                   _,
                                   _,
                                   _,
                                   _,
                                   ShuffleExchangeExec(
                                     _,
                                     HashAggregateExec(
                                       _,
                                       _,
                                       _,
                                       _,
                                       _,
                                       _,
                                       ProjectExec(
                                         _,
                                         FilterExec(
                                           _,
                                           FileSourceScanExec(
                                             fsRelation,
                                             outputAttributes,
                                             outputSchema,
                                             partitionFilters,
                                             optionalBucketSet,
                                             dataFilters,
                                             _
                                           )
                                         )
                                       )
                                     ),
                                     _)) =>
          if (fsRelation.fileFormat.isInstanceOf[ParquetFileFormat])
            FletcherParquetSourceScanExec(
              fsRelation,
              f.output,
              //expressions,
              outputSchema,
              partitionFilters,
              optionalBucketSet,
              dataFilters
            )
          else f
      }
    }
    //override def postColumnarTransitions: Rule[SparkPlan] = {
    //  case p @ HashAggregateExec(
    //        _,
    //        _,
    //        _,
    //        _,
    //        _,
    //        _,
    //        ShuffleExchangeExec(
    //          _,
    //          HashAggregateExec(
    //            _,
    //            _,
    //            _,
    //            _,
    //            _,
    //            _,
    //            ProjectExec(_, FilterExec(_, child))
    //          ),
    //          _
    //        )
    //      ) =>
    //    ProjectExec(p.output, postColumnarTransitions(child))
    //  //case p@ProjectExec(_, FilterExec(_, ColumnarToRowExec(child))) =>
    //  //  FletcherReductionExampleExec(p.output, postColumnarTransitions(child))
    //  case plan =>
    //    plan.withNewChildren(plan.children.map(postColumnarTransitions(_)))
    //}
  }
}
