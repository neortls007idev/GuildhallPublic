// BlockingQueue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <thread>
#include <atomic>

//--------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class  SynchronizedLockFreeQueue : protected std::queue<T>
{
public:
	using valueType = typename T;
	
protected:
	using Base = std::queue<T>;
	
public:
	SynchronizedLockFreeQueue() : Base() {}
	SynchronizedLockFreeQueue( SynchronizedLockFreeQueue && ) = delete;
	SynchronizedLockFreeQueue( SynchronizedLockFreeQueue const& ) = delete;

	~SynchronizedLockFreeQueue() = default;

	SynchronizedLockFreeQueue& operator=( SynchronizedLockFreeQueue const& ) = delete;
	SynchronizedLockFreeQueue& operator=( SynchronizedLockFreeQueue&& ) = delete;

	void push( valueType const& value );
	valueType pop();

protected:
	void Lock();
	void Unlock();
	
private:
	const int UNLOCKED = 0;
	const int LOCKED = 1;
	std::atomic<int> m_atomicLock;
};

//--------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
void SynchronizedLockFreeQueue<T>::push( valueType const& value )
{
	Lock();
	Base::push( value );
	Unlock();
	
}

//--------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
typename SynchronizedLockFreeQueue<T>::valueType SynchronizedLockFreeQueue<T>::pop()
{
	valueType value = valueType( 0 );

	Lock();
	if ( !Base::empty() )
	{
		value = Base::front();
		Base::pop();
	}
	Unlock();
	return value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
void SynchronizedLockFreeQueue<T>::Lock()
{
	int expected = UNLOCKED;
	while( !m_atomicLock.compare_exchange_strong( expected , LOCKED ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
void SynchronizedLockFreeQueue<T>::Unlock()
{
	int expected = LOCKED;
	while ( !m_atomicLock.compare_exchange_strong( expected , UNLOCKED ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
class BlockingQueue : protected std::queue<T>
{
public:
//	typedef std::queue<T> baseQueue;
	using valueType = typename T;
protected:
	using baseQueue = typename std::queue<T>;
		
public:
	BlockingQueue() : baseQueue() {}
	BlockingQueue( BlockingQueue&& ) = delete;
	BlockingQueue( BlockingQueue const& ) = delete;
	~BlockingQueue() = default;

	BlockingQueue& operator=( BlockingQueue&& ) = delete;
	BlockingQueue& operator=( BlockingQueue const& ) = delete;
	
	void push( const valueType& value );
	valueType pop();
private:
	std::mutex m_lock;
	std::condition_variable m_condition;
};

//----------------------------------------------------------------------------------------------------------

template<typename T>
void BlockingQueue<T>::push( const valueType& value )
{
	std::lock_guard<std::mutex> guard( m_lock );
	baseQueue::push( value );
	m_condition.notify_all();
}

//----------------------------------------------------------------------------------------------------------

template<typename T>
typename BlockingQueue<T>::valueType BlockingQueue<T>::pop( )
{
	valueType value = valueType();

	std::unique_lock<std::mutex> uniqueLock( m_lock );
	if ( baseQueue::empty() )
	{
		m_condition.wait( uniqueLock );
	}
	
	value = baseQueue::front();
	baseQueue::pop();

	return value;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

template<typename QueueT>
void ReadQueue( QueueT& queue )
{
	int value = QueueT::valueType( 0 );
	do 
	{
		value = queue.pop();
		std::cout << std::this_thread::get_id() << "Popped value = " << value << std::endl;
	} while( value != QueueT::valueType( -1 ) );
	std::cout << std::this_thread::get_id() << "Exiting\n" << std::endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
	std::cout << "Main Thread =" << std::this_thread::get_id() << std::endl;
	
	BlockingQueue<int> blockingQueue;
	BlockingQueue<float> queueF;
	
	std::thread blockingReader( ReadQueue<BlockingQueue<int>> , std::ref( blockingQueue ) );
	blockingQueue.push( 5 );
	blockingQueue.push( 7 );
	blockingQueue.push( 37 );

	std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

	blockingQueue.push( 8 );
	blockingQueue.push( 9 );
	blockingQueue.push( 39 );
	blockingQueue.push( 21 );

	std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
	blockingQueue.push( -1 );

	blockingReader.join();
	//static constexpr bool is_always_lock_free = std::atomic<int>::is_always_lock_free( &is_always_lock_free );

	std::atomic<int> test;
	
	std::cout << "\n Int is lock free ? = " << test.is_lock_free() << std::endl; 
	SynchronizedLockFreeQueue<int> syncedQueue;
	
	std::thread syncedReader( ReadQueue<SynchronizedLockFreeQueue<int>> , std::ref( syncedQueue ) );

	for( int idx = 1; idx < 20; ++idx )
	{
		std::cout << std::this_thread::get_id() << "Pushing Value: " << idx << std::endl;
		syncedQueue.push( idx );
		if ( idx % 3 == 0 )
		{
			std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
		}
	}
	syncedQueue.push( -1 );
	
	syncedReader.join();	
}
