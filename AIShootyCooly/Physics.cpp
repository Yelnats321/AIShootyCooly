#include "stdafx.h"
#include "Physics.h"

Physics::Physics() {
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, 
									gDefaultErrorCallback);

	if (!foundation)
		throw "PxCreateFoundation failed!";

	/*auto profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);
	if (!profileZoneManager)
	throw "PxProfileZoneManager::createProfileZoneManager failed!";*/

	bool recordMemoryAllocations = true;
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), 
							  recordMemoryAllocations, nullptr);
	if (!physics)
		throw "PxCreatePhysics failed!";

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	if (!sceneDesc.cpuDispatcher) {
		cpuDispatcher = PxDefaultCpuDispatcherCreate(nbThreads);
		if (!cpuDispatcher)
			throw "PxDefaultCpuDispatcherCreate failed!";
		sceneDesc.cpuDispatcher = cpuDispatcher;
	}
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = &PxDefaultSimulationFilterShader;

	scene = physics->createScene(sceneDesc);
	if (!scene)
		throw "createScene failed!";
}

Physics::~Physics() {
	physics->release();
	foundation->release();
}

Physics & Physics::getInstance() {
	static Physics physics;
	return physics;
}