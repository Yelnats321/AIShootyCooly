#include "stdafx.h"
#include "Physics.h"

Physics::Physics() {
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;
	foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, 
									gDefaultErrorCallback);

	if (!foundation_)
		throw std::runtime_error("PxCreateFoundation failed!");

	/*auto profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);
	if (!profileZoneManager)
	throw "PxProfileZoneManager::createProfileZoneManager failed!";*/

	bool recordMemoryAllocations = true;
	physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation_, PxTolerancesScale(), 
							   recordMemoryAllocations, nullptr);
	if (!physics_)
		throw std::runtime_error("PxCreatePhysics failed!");

	PxSceneDesc sceneDesc(physics_->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	if (!sceneDesc.cpuDispatcher) {
		cpuDispatcher_ = PxDefaultCpuDispatcherCreate(NbThreads_);
		if (!cpuDispatcher_)
			throw std::runtime_error("PxDefaultCpuDispatcherCreate failed!");
		sceneDesc.cpuDispatcher = cpuDispatcher_;
	}
	if (!sceneDesc.filterShader) {
		sceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	}
	scene_ = physics_->createScene(sceneDesc);
	if (!scene_)
		throw std::runtime_error("createScene failed!");
}

Physics::~Physics() {
	physics_->release();
	foundation_->release();
}

Physics & Physics::getInstance() {
	static Physics physics;
	return physics;
}

void Physics::step(double dt) {
	static double accumulator = 0.0f;
	static double stepSize = 1.0f / 60.0f;
	accumulator += dt;
	if (accumulator < stepSize) return;
	accumulator -= stepSize;
	
	scene_->simulate(static_cast<PxReal>(stepSize));
	scene_->fetchResults(true);
}
