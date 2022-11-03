#ifndef CNET_H_
#define CNET_H_

#include <thread>
#include <vector>
#include <list>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <atomic>
#include <deque>
#include <functional>

class RequestTask;
class Response
{
	friend class RequestTask;
private:
	std::string data;
	std::mutex mtx;
	std::condition_variable_any cv;
	bool ready;
	void emitData(std::string data);
public:
	std::string GetData();
};

class Task {

public:
	typedef std::shared_ptr<Task> Ptr;
	virtual void Do() = 0;
	virtual ~Task() {}
};

class RequestTask : public Task, public std::enable_shared_from_this<RequestTask> {

private:
	std::string host;
	int port; 
	std::string request;
	std::shared_ptr<Response> resp;
public:
	typedef std::shared_ptr<RequestTask> Ptr;

	RequestTask(const std::string& host, int port, const std::string& request, std::shared_ptr<Response>& resp) :
		host(host), port(port), request(request), resp(resp) {
	}

	static RequestTask::Ptr New(const std::string& host, int port, const std::string& request, std::shared_ptr<Response>& resp) {
		return RequestTask::Ptr(new RequestTask(host,port,request,resp));
	}

	void Do();
};


class ClosureTask : public Task, public std::enable_shared_from_this<ClosureTask> {

public:
	using Callback = std::function<void(void)>;
	typedef std::shared_ptr<ClosureTask> Ptr;

	template<typename F, typename ...TArgs>
	static ClosureTask::Ptr New(F&& callback, TArgs&& ...args) {
		return ClosureTask::Ptr(new ClosureTask(std::bind(std::forward<F>(callback), std::forward<TArgs>(args)...)));
	}

	void Do() {
		mCallback();
	}
private:

	explicit ClosureTask(const Callback& cb) :mCallback(cb) {

	}

	Callback mCallback;
};


class ThreadPool final {

	//任务队列
	class TaskQueue {

	public:
		typedef std::deque<Task::Ptr> taskque;

		TaskQueue() :closed(false), watting(0) {}

		void PostTask(const Task::Ptr& task);

		void Close();

		Task::Ptr Get();

		bool Get(taskque& out);

	private:
		TaskQueue(const TaskQueue&) = delete;
		TaskQueue& operator = (const TaskQueue&) = delete;
		bool closed;
		int  watting;//空闲线程数量
		std::mutex mtx;
		std::condition_variable_any cv;
		taskque tasks;
	};

public:

	static const int SwapMode = 1;

	ThreadPool() :inited(false) {}

	~ThreadPool();

	bool Init(int threadCount = 0, int swapMode = 0);

	void PostTask(const Task::Ptr& task) {
		queue_.PostTask(task);
	}

	template<typename F, typename ...TArgs>
	void PostClosure(F&& callback, TArgs&& ...args) {
		queue_.PostTask(ClosureTask::New(std::forward<F>(callback), std::forward<TArgs>(args)...));
	}

	void Close() {
		queue_.Close();
	}

private:

	static void threadFunc(ThreadPool::TaskQueue* queue_);
	static void threadFuncSwap(ThreadPool::TaskQueue* queue_);

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator = (const ThreadPool&) = delete;

	std::atomic_bool inited;
	TaskQueue queue_;
	std::vector<std::thread> threads_;
};


class CNet
{
private:
	static ThreadPool threadpool;
public:
	static int InitNetSystem();
	static std::shared_ptr<Response> Request(const std::string &host, int port, const std::string &request);

};


#endif