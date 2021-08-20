#include "ColumnScheduler.h"


namespace tpc 
{

		double ColumnScheduler::Schedule(const std::vector<std::vector<PtoaRegs>>& chunks)
                {
			double result = 0;
			std::vector<std::future<double>> futures(chunks.size());
			//printf("[THREADS DEBUG]: %d number of tasks will be submitted\n", chunks.size());
			for (int i = 0; i < chunks.size(); ++i)
			{
				//printf("[TASK NO] Reg is : %lu\n", (uint64_t) chunks[i][0].device_parquet_address);
				//futures[i] = pool.submit(std::bind(&ColumnScheduler::RunInstanceProjection, this, std::move(chunks[i])));
			}
			for (int i = 0; i < chunks.size(); ++i)
			{
				result += futures[i].get();
			}
			return result;
                }
		fletcher::Status ColumnScheduler::Submit(int chunk_size, PtoaRegs** chunks)
		{
			std::vector<std::future<double>> futures(chunk_size);
			//futures.resize(chunks.size());
			//printf("[THREADS DEBUG]: %d number of tasks will be submitted\n", chunk_size);
			for (int i = 0; i < chunk_size; ++i)
			{       
                                PtoaRegs* chunks_to_send = chunks[i];
				//printf("[TASK NO] Reg is : %lu\n", (uint64_t) chunks_to_send->device_parquet_address);
				futures[i] = pool.submit(std::bind(&ColumnScheduler::RunInstanceProjection, this, chunks_to_send));
			}
                        for(auto & f: futures)
                        {
                            results.push_back(f.get()); 
                        }
                        tasks_submitted = true;
                        return fletcher::Status::OK();
		}
		double ColumnScheduler::Next()
		{
                        while(!tasks_submitted)
                        {} 
                        auto result = results.front(); 
                        results.pop_front();
                        return result;
			//auto it_future = futures.begin();
			// We iterate until future which is not ready
			//std::future<double> curr;
                        //double res;
                        //for(auto & f: futures)
                        //{
                        //    results.push_back(f.get()); 
                        //}
			//while (!future_is_ready(*it_future))
			//{
                        //        std::cout << "next future\n";
                        //        curr = std::move(*it_future);
                        //        res = curr.get();
			//	// Go to next future
			//	it_future++;
			//}
			//futures.erase(it_future);
			//return res;
		}
		bool ColumnScheduler::hasNext()
		{
			return (results.size() > 0) and tasks_submitted ;
		}
		bool ColumnScheduler::future_is_ready(std::future<double> const &f)
		{
			return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		}

}
