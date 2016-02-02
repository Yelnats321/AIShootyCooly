#include "stdafx.h"
#include "Renderer.h"

#include <vld.h>

using namespace physx;

class UserErrorCallback : public PxErrorCallback {
	PxDefaultErrorCallback callback;
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {
		// error processing implementation
		callback.reportError(code, message, file, line);
	}
};


int main() {
	static UserErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	auto foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	if (!foundation)
		throw "PxCreateFoundation failed!";

	/*auto profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);
	if (!profileZoneManager)
		throw "PxProfileZoneManager::createProfileZoneManager failed!";*/

	bool recordMemoryAllocations = true;
	auto physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
								   PxTolerancesScale(), recordMemoryAllocations, nullptr);
	if (!physics)
		throw "PxCreatePhysics failed!";

	Renderer renderer;

	while (renderer.isOpen()) {
		renderer.draw();
	}

	physics->release();
	//profileZoneManager->release();
	foundation->release();
	return 0;
}

