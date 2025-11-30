#pragma once

#include <atomic>
#include <tuple>

namespace lfmq
{
/*
 * the lock free queue implementation is a circular buffer where the value is
 * never actually "popped" but rather overwritten. the read and write index
 * both start at 0, and the write index is incremented when a value is pushed
 * to the queue. To check whether the queue is full when pushing, you check
 * the index which you will be writing to. If it is equal to the read index,
 * then the queue is full. Else, write to that index and increment the write
 * index member variable
 */
template <typename _T, size_t _size> requires std::is_default_constructible_v<_T> && (_size > 2)
class SpscQueue {
public:
	/**
	 * @brief Insert an element onto the queue
	 * @note Only call this from the producer thread
	 * @param element Element to be inserted onto the queue
	 * @return Whether the element was successfully inserted onto the queue
	 */
	bool push(const _T& element) {
		return this->_push(element);
	}
	/**
	 * @brief Insert an element onto the queue
	 * @note Only call this from the producer thread
	 * @param element Element to be inserted onto the queue
	 * @return Whether the element was successfully inserted onto the queue
	 */
	bool push(_T&& element) noexcept {
		return this->_push(std::move(element));
	}

	/**
	 * @brief Remove the oldest element from the queue
	 * @note Only call this from the consumer thread
	 * @param element Pointer to assign value of the element at the front to. nullptr if retrieving the element is not desired. Will not be modified if pop returns false
	 * @return True if the queue has elements and value was popped, false if the queue is empty
	 */
	bool pop(_T* const element = nullptr) {
		size_t curr_read_index = this->read_index.load();
		const size_t curr_write_index = this->write_index.load();

		// queue is empty
		if (curr_read_index == curr_write_index) {
			return false;
		}

		if (element != nullptr) {
			*element = this->elements[curr_read_index];
		}

		curr_read_index++;
		if (curr_read_index == this->capacity()) {
			curr_read_index = 0;
		}

		this->read_index.store(curr_read_index);

		return true;
	}

	/**
	 * @brief Return a const reference to the element at the start of the queue
	 * @note Only call this from the consumer thread
	 * @return Const reference to the element at the start of the queue
	 */
	const _T& front() const noexcept {
		return this->elements[this->read_index.load()];
	}

	/**
	 * @brief Return a reference to the element at the start of the queue
	 * @note Only call this from the consumer thread
	 * @return Reference to the element at the start of the queue
	 */
	_T& front() noexcept {
		return this->elements[this->read_index.load()];
	}

	/**
	 * @brief Return the max size of the queue
	 * @return Max size of the queue
	 */
	constexpr size_t capacity() const noexcept {
		return _size;
	}

	/**
	 * @brief Return whether the queue is empty
	 * @return Whether the queue is empty
	 */
	bool is_empty() const noexcept {
		return this->read_index.load() == this->write_index.load();
	}

private:
	/**
	 * @brief Insert an element onto the queue
	 * @note Only call this from the producer thread
	 * @param element Forwarding reference element to be inserted onto the queue
	 * @return Whether the element was successfully inserted onto the queue
	 */
	template<typename _fr_T>
	bool _push(_fr_T&& element) {
		// TODO read more of this page to optimize memory order https://en.cppreference.com/w/cpp/atomic/memory_order
		const size_t curr_write_index = this->write_index.load();
		size_t next_write_index = curr_write_index + 1;

		if (next_write_index == this->capacity()) {
			next_write_index = 0;
		}

		// queue is full
		if (this->read_index.load() == next_write_index) {
			return false;
		}

		this->elements[curr_write_index] = std::forward<_fr_T>(element);
		this->write_index.store(next_write_index);

		return true;
	}

	_T elements[_size];

	std::atomic<size_t> read_index  = 0;
	std::atomic<size_t> write_index = 0;
};
} // namespace lfmq

/// Tuple size specialization for SpscQueue
template<typename _T, size_t _size>
struct std::tuple_size<lfmq::SpscQueue<_T, _size>> : public std::integral_constant<std::size_t, _size>
{};
