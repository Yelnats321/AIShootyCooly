#pragma once

using namespace physx;

class Physics {
	static const int nbThreads = 4;
	PxFoundation * foundation;
	PxPhysics * physics;
	PxScene * scene;
	PxCpuDispatcher * cpuDispatcher;
	Physics();
public:
	~Physics();

	static Physics & getInstance();

	void step(double);
};
