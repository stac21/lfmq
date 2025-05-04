#pragma once

#include <cstring>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace lfmq
{
enum class MessageType {
	UNKNOWN,         // Unknown message type
	RESUME,          // Resume the audio stream
	PAUSE,           // Pause the audio stream
	STOP,            // Stop audio playback and shut down the audio thread
	VOLUME,          // Adjust the volume of the audio stream
	RESIZE,          // Inform the audio thread that one of its dynamic buffers has been resized on the controller thread
	EFFECT_ADDED,    // A new effect has been added by the user
	EFFECT_REMOVED,  // An effect has been removed by the user
	EFFECT_ENABLED,  // Enable an effect
	EFFECT_DISABLED, // Disable an effect
	PLAY_AT          // begin playing at specific time or frame index
};

class MessageMetadata {
private:
	MessageType m_type;

public:
	constexpr MessageMetadata() noexcept :
		m_type(MessageType::UNKNOWN)
	{ }

	constexpr MessageMetadata(const MessageType type) noexcept :
		m_type(type)
	{ }

	constexpr MessageType get_type() const noexcept {
		return this->m_type;
	}

	void set_type(const MessageType type) noexcept;
};

class Message {
public:
	/// Half of 1 Kb to be safe. This can be reevaluated later if needs be
	static constexpr size_t MAX_MESSAGE_SIZE = 512;

private:
	MessageMetadata metadata;
	char            payload[MAX_MESSAGE_SIZE];
	size_t          payload_size;

	/**
	 * @brief Copy the size in bytes specified by size from data into the payload
	 * @param data Source of the data payload
	 * @param size Size in bytes to copy from the data payload
	 * @return True if copy was successful, false if data was a nullptr or the size was greater than MAX_MESSAGE_SIZE
	 */
	bool set_payload(const void* const data, const size_t size);

public:
	constexpr Message() :
		metadata(),
		payload{ 0 },
		payload_size(0)
	{ }

	template<typename _T> requires (sizeof(_T) <= MAX_MESSAGE_SIZE)
	Message(const MessageMetadata& metadata, _T&& data) :
		metadata(metadata),
		payload(),
		payload_size() {

		/*
		 * The size of data is already checked at compile-time, so the only
		 * way this can fail is if data == nullptr
		 */
		bool set_payload_result = this->set_payload(data);

		if constexpr (std::is_pointer_v<_T>) {
			if (!set_payload_result) {
				throw std::runtime_error("Null pointer passed in as data");
			}
		}
	}

	~Message() = default;

	constexpr const MessageMetadata& get_metadata() const noexcept {
		return this->metadata;
	}
	constexpr const char* get_payload() const noexcept {
		return this->payload;
	}

	template<typename _T> requires (sizeof(_T) <= MAX_MESSAGE_SIZE)
	constexpr const _T& get_payload() const noexcept {
		/*
		* in the case of T deducing to a pointer type, this will treat
		* the message payload as a T**
		*/
		return *reinterpret_cast<const _T*>(this->payload);
	}

	constexpr size_t get_payload_size() const noexcept {
		return this->payload_size;
	}

	void set_metadata(const MessageMetadata& metadata) noexcept;

	/**
	 * @brief Set the payload to the passed in data
	 * @param data Data to copy into the payload
	 * @eturn True if copy was successful, false if data was nullptr
	 */
	template<typename _T> requires (sizeof(_T) <= MAX_MESSAGE_SIZE)
	bool set_payload(_T&& data) {
		if constexpr (std::is_pointer_v<_T>) {
			if (data == nullptr) {
				return false;
			}
		}

		/*
		 * in the case of T deducing to a pointer type, this will treat
		 * the message payload as a T**
		 */
		return this->set_payload(&data, sizeof(_T));
	}

	friend void swap(Message& lhs, Message& rhs) noexcept {
		using std::swap;

		swap(lhs.metadata, lhs.metadata);
		swap(lhs.payload, rhs.payload);
		swap(lhs.payload_size, rhs.payload_size);
	}
};
} // namespace lfmq