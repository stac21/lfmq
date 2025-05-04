#include "message.hpp"

namespace lfmq
{
/*
 * Start MessageMetadata class definitions
 */
void MessageMetadata::set_type(const MessageType type) noexcept {
	this->m_type = type;
}
/*
 * End MessageMetadata class definitions
 */

 /*
  * Start Message class definitions
  */
bool Message::set_payload(const void* const data, const size_t size) {
	if (data == nullptr || size > MAX_MESSAGE_SIZE) {
		return false;
	}

	memcpy(this->payload, data, size);
	this->payload_size = size;

	return true;
}

void Message::set_metadata(const MessageMetadata& metadata) noexcept {
	this->metadata = metadata;
}
/*
 * End Message class definitions
 */
} // namespace lfmq
