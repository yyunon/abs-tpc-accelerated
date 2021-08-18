#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <future>
#include <chrono>
#include <boost/atomic.hpp>
#include <condition_variable>
#include "Util.h"

namespace tpc
{
	//static thread_local uint32_t instance_index;
	extern __thread uint32_t instance_index;
	//extern std::condition_variable cv;
	//extern std::mutex mmio_mtx;
	//static bool block_mmio_1;
	//static bool block_mmio_2;
	namespace internal
	{
		//class FunctionWrapper;
		//class JoinThreads;
		//class WorkStealingTaskQueue;
		//class TaskQueue;
		//class ThreadPool;
		//class PlatformWrapper;

		//We use this platform wrapper to lock platform instances so that make sure mmio regs is set carefully.
		class spinlock {
                  private:
                    typedef enum {Locked, Unlocked} LockState;
                    boost::atomic<LockState> state_;

                  public:
                    spinlock() : state_(Unlocked) {}

                    void lock()
                    {
                      while (state_.exchange(Locked, boost::memory_order_acquire) == Locked) {
                        /* busy-wait */
                      }
                    }
                    void unlock()
                    {
                      state_.store(Unlocked, boost::memory_order_release);
                    }
                };
		class ThreadSafePlatform
		{
			std::atomic_bool mmio_busy = false;
			std::shared_ptr<fletcher::Platform> platform_g;
                        spinlock s;
			//mutable std::mutex platform_mtx;
			//std::condition_variable mmio_condition;

		public:
			template <class Operation>
			ThreadSafePlatform() {};
			ThreadSafePlatform(bool start): mmio_busy(start), s() {}

			inline fletcher::Status Reset(std::shared_ptr<fletcher::Platform> &platform, const std::vector<uint64_t> &base_offsets_)
			{
				platform_g = std::atomic_load(&platform);
                                s.lock();
				//std::unique_lock<std::mutex> platform_locker(platform_mtx);
				//mmio_condition.wait(platform_locker, [this]
														//{ return mmio_busy == false; });
				//platform_locker.lock();
                                //printf("[THREAD NO %d]: Locked...\n", instance_index);
				//mmio_busy = true;
				for (int i = 0; i < (NUM_COLS + 1); ++i) //Reset
				{
					platform_g->WriteMMIO(base_offsets_[i], 0, 0x04);
					platform_g->WriteMMIO(base_offsets_[i], 0, 0);
				}
				//platform_locker.unlock();
                                //printf("[THREAD NO %d]: Unlocked...\n", instance_index);
				//mmio_busy = false;
				//mmio_condition.notify_one();
                                s.unlock();
				return fletcher::Status::OK();
			}

			inline fletcher::Status Start(std::shared_ptr<fletcher::Platform> &platform, const std::vector<uint64_t> &base_offsets_)
			{
				//std::unique_lock<std::mutex> platform_locker(platform_mtx);
                                s.lock();
				//platform_g = std::atomic_load(&platform);
				//mmio_condition.wait(platform_locker, [this]
				//										{ return mmio_busy == false; });
				//platform_locker.lock();
				//mmio_busy = true;
                                //printf("[THREAD NO %d]: Locked...\n", instance_index);
				for (int i = NUM_COLS; i >= 0; --i) //Start
				{
					platform_g->WriteMMIO(base_offsets_[i], 0, 0x01);
					platform_g->WriteMMIO(base_offsets_[i], 0, 0);
				}
                                s.unlock();
				//platform_locker.unlock();
				//mmio_busy = false;
                                //printf("[THREAD NO %d]: Unlocked...\n", instance_index);
				//mmio_condition.notify_one();
				return fletcher::Status::OK();
			}
			inline fletcher::Status WaitForFinish(std::shared_ptr<fletcher::Platform> &platform, uint64_t offset, unsigned int poll_interval_usec)
			{
				platform_g = std::atomic_load(&platform);
				//FLETCHER_LOG(DEBUG, "Polling kernel for completion.");
				uint32_t status = 0;
				uint32_t done_status = 1ul << 0x2u;
				uint32_t done_status_mask = 1ul << 0x2u;
				if (poll_interval_usec == 0)
				{
					do
					{
						platform_g->ReadMMIO(offset, 0x01, &status);
					} while ((status & done_status_mask) != done_status);
				}
				else
				{
					do
					{
                                                //std::unique_lock<std::mutex> platform_locker(platform_mtx);
						//std::this_thread::sleep_for(std::chrono::milliseconds(poll_interval_usec));
						//mmio_condition.wait(platform_locker, [this]
						//										{ return mmio_busy == false; });
						////platform_locker.lock();
						//mmio_busy = true;
                                                //printf("[THREAD NO %d]: Locked...\n", instance_index);
                                                s.lock();
						platform_g->ReadMMIO(offset, 0x01, &status);
                                s.unlock();
						//platform_locker.unlock();
                                                //printf("[THREAD NO %d]: Unlocked...\n", instance_index);
						//mmio_busy = false;
						//mmio_condition.notify_one();
					} while ((status & done_status_mask) != done_status);
				}
				//FLETCHER_LOG(DEBUG, "Kernel status done bit asserted.");
				return fletcher::Status::OK();
			}

			inline double ReadResult(std::shared_ptr<fletcher::Platform> &platform, uint64_t offset, unsigned int poll_interval_usec)
			{
				platform_g = std::atomic_load(&platform);
				uint32_t rlow, rhigh;
				//std::unique_lock<std::mutex> platform_locker(platform_mtx);
				//mmio_condition.wait(platform_locker, [this]
														//{ return mmio_busy == false; });
				//platform_locker.lock();
				//mmio_busy = true;
                                s.lock();
				platform_g->ReadMMIO(offset, 0xF, &rlow);
				platform_g->ReadMMIO(offset, 0xE, &rhigh);
                                s.unlock();
				//platform_locker.unlock();
				//mmio_busy = false;
				//mmio_condition.notify_one();
				uint64_t result_r = rhigh;
				result_r = (result_r << 32) | rlow;
				return double(result_r) / double(1 << 18);
			}
			inline fletcher::Status setPtoaArguments(std::shared_ptr<fletcher::Platform> &platform, const std::vector<uint64_t> &base_offsets_, PtoaRegs *regs)
			{
				platform_g = std::atomic_load(&platform);
				//std::unique_lock<std::mutex> platform_locker(platform_mtx);
				//mmio_condition.wait(platform_locker, [this]
														//{ return mmio_busy == false; });
				//platform_locker.lock();
				//mmio_busy = true;
                                s.lock();
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
                                s.unlock();
				//platform_locker.unlock();
				//mmio_busy = false;
				//mmio_condition.notify_one();
				return fletcher::Status::OK();
			}
                        template <class Operation>
			auto call(Operation o) -> decltype(o(platform_g))
			{
				//std::unique_lock<std::mutex> platform_locker(platform_mtx);
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
		internal::ThreadSafePlatform p_w;
		std::shared_ptr<fletcher::Platform> platform;
		inline double RunInstanceProjection(PtoaRegs *regs)
		{
                        std::this_thread::sleep_for(std::chrono::milliseconds(10 * instance_index ));
			//printf("[THREAD NO %d]: Task running...\n", instance_index);
			//We calculate the base offsets.
			std::vector<uint64_t> base_offsets_;
			for (int i = 0; i < (NUM_COLS + 1); ++i)
				base_offsets_.push_back(tpc::calculate_reg_base_offset(i + (NUM_COLS + 1) * instance_index));
			ASSERT_FLETCHER_OK(p_w.Reset(platform, base_offsets_));
			ASSERT_FLETCHER_OK(p_w.setPtoaArguments(platform, base_offsets_, regs));
			ASSERT_FLETCHER_OK(p_w.Start(platform, base_offsets_));
			ASSERT_FLETCHER_OK(p_w.WaitForFinish(platform, base_offsets_[NUM_COLS], instance_index * 500));
			double result = p_w.ReadResult(platform, base_offsets_[NUM_COLS], instance_index * 100);
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

		double Schedule(const std::vector<std::vector<PtoaRegs>> &chunks);
		fletcher::Status Submit(int chunk_size, PtoaRegs **chunks);
		bool future_is_ready(std::future<double> const &f);
		bool hasNext();
		double Next();

		~ColumnScheduler() = default ;
		//ColumnScheduler (const ColumnScheduler&) = delete;
		//ColumnScheduler& operator= (const ColumnScheduler&) = delete;

		ColumnScheduler(const std::shared_ptr<fletcher::Platform> &platform_) : p_w(false), platform(platform_)
		{
		}

	private:
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
