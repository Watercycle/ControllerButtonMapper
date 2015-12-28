#pragma once

extern long long unsigned eventUID;

template <typename TEventType>
struct EventHandle
{
	unsigned long id_;
	std::function<void(const TEventType&)> handle_;
	unsigned repeat_;

	EventHandle(std::function<void(const TEventType&)> handle, unsigned repeat = 0) :
		handle_(handle), repeat_(repeat), id_(eventUID++) {}

	bool operator==(const EventHandle& other) { return id_ == other.id_; }
};

class Events
{
public:
	Events();
	~Events();

public:
	template <typename TEventType>
	static void add(EventHandle<TEventType> handler) {
		InternalChannel<TEventType>::instance().add(handler);
	}

	template <typename TEventType>
	static void remove(EventHandle<TEventType> handler) {
		InternalChannel<TEventType>::instance().remove(handler);
	}
	
	template <typename TEventType>
	static void broadcast(const TEventType& message) {
		InternalChannel<TEventType>::instance().broadcast(message);
	}
public:

	//an internal channel is used to allow the vector of events to be templated
	template <typename TEventType>
	class InternalChannel
	{
	private:
		std::mutex mutex_;
	    std::vector<EventHandle<TEventType>> handlers_;
		InternalChannel() {} //hide the internal event channel

	public: 
		static InternalChannel& instance() { 
			static InternalChannel result;
			return result; 
		}

		void add(EventHandle<TEventType> handler) {
			std::lock_guard<std::mutex> lock(mutex_);
			handlers_.push_back(handler);
		}

		void remove(EventHandle<TEventType> handler) {
			std::lock_guard<std::mutex> lock(mutex_);
			handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler), handlers_.end());
		}

		void broadcast(const TEventType& message) {
			std::lock_guard<std::mutex> lock(mutex_);
			auto localHandlers = handlers_;

			for (auto& handler : localHandlers) {
				handler.handle_(message);

				if (handler.repeat_ == 1) remove(handler);
				else handler.repeat_--;
			}
		}
	};
};

struct ButtonPress { int code; };
struct ButtonRelease { int code; };
struct MouseScroll { double xOffset; double yOffset; };
struct MousePress { int button; };
struct MouseRelease { int button; };