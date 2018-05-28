#include "RenderPayload.h"

namespace valkyrie
{
	static mutex payloadMutex;
	static ESPPayload globalPayload = { 0 };
	static ESPPayload bufferPayload = { 0 };

	auto copyBufferToPayload() -> void
	{
		std::lock_guard<mutex> payloadLock(payloadMutex);
		memcpy(&globalPayload, &bufferPayload, sizeof(ESPPayload));
	}

	auto disablePayload() -> void
	{
		std::lock_guard<mutex> payloadLock(payloadMutex);
		globalPayload.shouldDraw = false;
	}

	auto copyOutPayload(ESPPayload& copyOut) -> void
	{
		std::lock_guard<mutex> payloadLock(payloadMutex);
		memcpy(&copyOut, &globalPayload, sizeof(ESPPayload));
	}

	auto getPayloadBuffer() -> ESPPayload&
	{
		return bufferPayload;
	}
}