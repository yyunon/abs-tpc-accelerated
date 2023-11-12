# abs-tpc-accelerated

Concurrent FPGA wrapper and Spark integration of accelerated parquet file reading while pushing down queries. This work is demonstrated as a part of thesis done in [ABS group TU Delft](http://resolver.tudelft.nl/uuid:85d80b28-f1ed-4e52-b233-1c20a7ba376b). The previous work related to this (and also setup on Power 9) repository can be found in https://github.com/fnonnenmacher/spark-arrow-accelerated. 

# Components
**fletcher**, is used for interfacing purposes with the hardware. It is the projects developed by ABS group.
**native_root** is the native functionality used to interface the underlying platform. It can be any FPGA. It dispatches multiple threads that communicates with JVM engine. 
**root** is the scala functionality that extens Apache Spark and communicates with JVM Native functions. 
