#pragma once
#include "CoreMinimal.h"
#include "HAL/ThreadSafeBool.h"
#include "Templates/Atomic.h"
#include "Templates/SharedPointer.h"

namespace avoid
{
	namespace rpc
	{
		class RPCServer;
	}
} // namespace avoid

class AVOID_API Server
{

public:
	Server();
	~Server();

	void Start(uint16_t port);
	void Stop();

	void SetWorld(UWorld *world);
	void WorldLoadingFinished();

private:
	void BindCallbacks();

private:
	avoid::rpc::RPCServer *server;
	UWorld *World;
	FThreadSafeBool WorldReady = false;
};
