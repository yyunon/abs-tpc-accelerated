#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <future>
#include <condition_variable>
#include "Util.h"

namespace tpc
{
	//static thread_local uint32_t instance_index;
	extern __thread uint32_t instance_index;
	namespace internal
	{
		//class FunctionWrapper;
		//class JoinThreads;
		//class WorkStealingTaskQueue;
		//class TaskQueue;
		//class ThreadPool;
		//class PlatformWrapper;

		//We use this platform wrapper to lock platform instances so that make sure mmio regs is set carefully.
		class PlatformWrapper
		{
			std::shared_ptr<fletcher::Platform> platform_g;
			mutable std::mutex platform_mtx;

		public:
			template <class Operation>
			auto call(Operation o) -> decltype(o(platform_g))
			{
				std::unique_lock<std::mutex> platform_locker(platform_mtx);
				return o(platform_g);
			}
		};
		// C++ Concurrency in Action
		class FunctionWrapper
		{
			struct impl_base
			{
				virtual void call() = 0;
				virtual ~impl_base() {}
			};
			std::unique_ptr<impl_base> impl;

			template <typename F>
			struct impl_type : impl_base
			{
				F f;
				impl_type(F &&f_) : f(std::move(f_)) {}
				void call() { f(); }
			};

		public:
			template <typename F>
			FunctionWrapper(F &&f) : impl(new impl_type<F>(std::move(f)))
			{
			}
			void operator()() { impl->call(); }
			FunctionWrapper() = default;
			FunctionWrapper(FunctionWrapper &&other) : impl(std::move(other.impl)) {}
			FunctionWrapper &operator=(FunctionWrapper &&other)
			{
				impl = std::move(other.impl);
				return *this;
			}
			FunctionWrapper(const FunctionWrapper &) = delete;
			FunctionWrapper(FunctionWrapper &) = delete;
			FunctionWrapper &operator=(const FunctionWrapper &) = delete;
		};
		// Makes sure threads joined on destruction
		class JoinThreads
		{
			std::vector<std::thread> &threads;

		public:
			explicit JoinThreads(std::vector<std::thread> &threads_) : threads(threads_) {}
			~JoinThreads()
			{
				for (uint64_t i = 0; i < threads.size(); ++i)
				{
					if (threads[i].joinable())
						threads[i].join();
				}
			}
		};
		class WorkStealingTaskQueue
		{
			typedef FunctionWrapper data_type;

		public:
			WorkStealingTaskQueue() = default;
			WorkStealingTaskQueue(const WorkStealingTaskQueue &other) = delete;
			WorkStealingTaskQueue &operator=(const WorkStealingTaskQueue &other) = delete;
			inline void push(data_type val)
			{
				std::lock_guard<std::mutex> locker(mtx);
				thread_queue.push_front(std::move(val));
			}
			inline bool try_pop(data_type &val)
			{
				std::lock_guard<std::mutex> locker(mtx);
				if (thread_queue.empty())
					return false;
				val = std::move(thread_queue.front());
				thread_queue.pop_front();
				return true;
			}
			inline bool try_steal(data_type &val)
			{
				std::lock_guard<std::mutex> locker(mtx);
				if (thread_queue.empty())
					return false;
				val = std::move(thread_queue.front());
				thread_queue.pop_back();
				return true;
			}
			inline int size() const
			{
				std::lock_guard<std::mutex> locker(mtx);
				return thread_queue.size();
			}
			inline bool empty() const
			{
				std::lock_guard<std::mutex> locker(mtx);
				return thread_queue.empty();
			}

		private:
			mutable std::mutex mtx; // Ret by val., mutable for const objects
			std::deque<data_type> thread_queue;
		};

		template <class T>
		class TaskQueue
		{
		public:
			TaskQueue() = default;
			inline void push(T val)
			{
				std::lock_guard<std::mutex> locker(mtx);
				thread_queue.push(std::move(val));
				thread_cond.notify_one();
			}
			inline bool try_pop(T &val)
			{
				std::unique_lock<std::mutex> locker(mtx);
				if (thread_queue.empty())
					return false;
				val = std::move(thread_queue.front());
				thread_queue.pop();
				return true;
			}
			inline int size() const
			{
				return thread_queue.size();
			}
			inline bool empty() const
			{
				std::lock_guard<std::mutex> locker(mtx);
				return thread_queue.empty();
			}

		private:
			mutable std::mutex mtx; // Ret by val., mutable for const objects
			std::queue<T> thread_queue;
			std::condition_variable thread_cond;
		};

		class ThreadPool
		{
			typedef FunctionWrapper task_type;
			std::atomic_bool done;
			std::vector<std::thread> threads;
			JoinThreads joiner;
			TaskQueue<FunctionWrapper> global_worker_queue;
			std::vector<std::unique_ptr<WorkStealingTaskQueue>> queues;
			//static thread_local WorkStealingTaskQueue *local_worker_queue;
			static __thread WorkStealingTaskQueue *local_worker_queue;

			void worker_thread(uint32_t index_)
			{
				instance_index = index_;
				local_worker_queue = queues[instance_index].get();
				while (!done)
				{
					run_pending_task();
				}
			}
			bool pop_task_from_local_queue(task_type &task)
			{
				return local_worker_queue && local_worker_queue->try_pop(task);
			}
			bool pop_task_from_global_worker_queue(task_type &task)
			{
				return global_worker_queue.try_pop(task);
			}
			bool pop_task_from_other_worker_queue(task_type &task)
			{
				for (uint32_t i = 0; i < queues.size(); ++i)
				{
					uint32_t const index = (instance_index + i + 1) % (queues.size());
					if (queues[index]->try_steal(task))
					{
						return true;
					}
				}
				return false;
			}

		public:
			ThreadPool() : done(false), joiner(threads)
			{
				const uint32_t thread_count = NUM_THREADS;

				//printf("[THREADS DEBUG]: %d threads dispatched.\n", thread_count);

				try
				{
					for (uint32_t i = 0; i < thread_count; ++i)
					{
						queues.push_back(std::unique_ptr<WorkStealingTaskQueue>(new WorkStealingTaskQueue));
					}
					for (uint32_t i = 0; i < thread_count; ++i)
					{
						threads.push_back(std::thread(&ThreadPool::worker_thread, this, i));
					}
				}
				catch (const std::exception &e)
				{
					done = true;
					std::cerr << e.what() << '\n';
				}
			}

			~ThreadPool() { done = true; }

			inline int size()
			{
				return global_worker_queue.size();
			}
			template <class FunctionType>
			std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f)
			{
				//Infer Future type
				typedef typename std::result_of<FunctionType()>::type result_type;
				std::packaged_task<result_type()> task(std::move(f));
				std::future<result_type> res(task.get_future());

				if (local_worker_queue)
					local_worker_queue->push(std::move(task));
				else
					global_worker_queue.push(std::move(task));
				return res;
			}
			void run_pending_task()
			{
				task_type task;
				// Column readers check if they have columnar chunks to read on their local queues first, then global queues, then the other columnar readers.
				if (pop_task_from_local_queue(task) || pop_task_from_global_worker_queue(task) || pop_task_from_other_worker_queue(task))
				{
					task();
				}
				else
				{
					std::this_thread::yield();
				}
			}
		};
	}

	// @brief
	// This will return busy instances so that hasNext can wrap around this instance.
	// We schedule instances(4 column readers + predicate/projection kernel)
	// We need to reset, set mmio registers of the instances,
	// We wait for projection instance then reset others.
	// Instance index is the index of 4 col. readers + projection. Base offsets is the offsets of each col reader or proj.
	// We do not want to attach these threads to JVM instantly, but as a future reference we may to implement the hasNext() in
	// Spark side. Row group readers are assigned to tasks in the thread pool.
	class ColumnScheduler
	{

		internal::PlatformWrapper p_w;
		inline double RunInstanceProjection(PtoaRegs* regs)
		{
			//printf("[THREAD NO %d]: Task running...\n", instance_index);
			double result = p_w.call([&](std::shared_ptr<fletcher::Platform> &platform_g)
															 {
																 //We calculate the base offsets.
																 std::vector<uint64_t> base_offsets_;
																 for (int i = 0; i < (NUM_COLS + 1); ++i) //Reset
																	 base_offsets_.push_back(tpc::calculate_reg_base_offset(i + (NUM_COLS + 1) * instance_index));

																 // Load the platform object
																 platform_g = std::atomic_load(&platform);

																 for (int i = 0; i < (NUM_COLS + 1); ++i) //Reset
																 {
																	 platform_g->WriteMMIO(base_offsets_[i], 0, 0x04);
																	 platform_g->WriteMMIO(base_offsets_[i], 0, 0);
																 }
																 for (int i = 0; i < NUM_COLS; ++i) //Set PTOA regs of column readers
																 {
																	 dau_t mmio64_writer;
																	 platform_g->WriteMMIO(base_offsets_[i], REG_BASE + 0, regs[i].num_val);

																	 mmio64_writer.full = (da_t)regs[i].device_parquet_address;
																	 platform_g->WriteMMIO(base_offsets_[i], REG_BASE + 1, mmio64_writer.lo);
																	 platform_g->WriteMMIO(base_offsets_[i], REG_BASE + 2, mmio64_writer.hi);

																	 mmio64_writer.full = regs[i].max_size;
																	 platform_g->WriteMMIO(base_offsets_[i], REG_BASE + 3, mmio64_writer.lo);
																	 platform_g->WriteMMIO(base_offsets_[i], REG_BASE + 4, mmio64_writer.hi);
																 }
																 for (int i = NUM_COLS; i >= 0; --i) //Start
																 {
																	 platform_g->WriteMMIO(base_offsets_[i], 0, 0x01);
																	 platform_g->WriteMMIO(base_offsets_[i], 0, 0);
																 }
																 // The last col. is the compute kernel we listen that one.
																 ASSERT_FLETCHER_OK(tpc::WaitForFinish(platform_g, base_offsets_[NUM_COLS], 151 + instance_index * 10));
																 uint32_t rlow, rhigh;
																 platform_g->ReadMMIO(base_offsets_[NUM_COLS], 0xF, &rlow);
																 platform_g->ReadMMIO(base_offsets_[NUM_COLS], 0xE, &rhigh);
																 uint64_t result_r = rhigh;
																 result_r = (result_r << 32) | rlow;
																 for (int i = 0; i < (NUM_COLS + 1); ++i) //Reset
																 {
																	 platform_g->WriteMMIO(base_offsets_[i], 0, 0x04);
																	 platform_g->WriteMMIO(base_offsets_[i], 0, 0);
																 }
																 return double(result_r) / (double)(1 << 18);
																 //tpc::ReadResultFloat(platform_g, base_offsets_[NUM_COLS], *result);
															 });
			//printf("[THREAD NO: %d]: Result is %f ...\n", instance_index, result);
			return result;
		}

	public:
		static fletcher::Status Make(std::shared_ptr<ColumnScheduler> *column_scheduler, const std::shared_ptr<fletcher::Platform> &platform_)
		{
			//printf("[THREADS DEBUG]: Making Scheduler instance \n");
			*column_scheduler = std::make_shared<ColumnScheduler>(platform_);
			//printf("[THREADS DEBUG]: Made Scheduler instance \n");
			return fletcher::Status::OK();
		};

		double Schedule(const std::vector<std::vector<PtoaRegs>>& chunks);
		fletcher::Status Submit(PtoaRegs** chunks);
		bool future_is_ready(std::future<double> const &f);
		bool hasNext();
		double Next();

                ~ColumnScheduler() = default;
                //ColumnScheduler (const ColumnScheduler&) = delete;
                //ColumnScheduler& operator= (const ColumnScheduler&) = delete;

		explicit ColumnScheduler(const std::shared_ptr<fletcher::Platform>& platform_) : platform(platform_)
		{
		}
		explicit ColumnScheduler(const std::string& platform_name)
		{
			//printf("[THREADS DEBUG]: Making Scheduler instance \n");
                        //printf("Creating platform\n");

                        ASSERT_FLETCHER_OK(fletcher::Platform::Make("snap", &platform, false));
                        ASSERT_FLETCHER_OK(platform->Init());
		}

	private:
		std::shared_ptr<fletcher::Platform> platform;
		std::vector<std::vector<PtoaRegs>> chunks;
		internal::ThreadPool pool;
		const int num_of_instances = NUM_THREADS;
		int num_of_busy_threads;
		int num_of_idle_threads;
                bool tasks_submitted = false;
                std::deque<double> results;
		std::deque<std::future<double>> futures;
	};
}
