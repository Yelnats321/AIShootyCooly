#pragma once

using namespace physx;

class Physics {
	static const int NumThreads_ = 4;
	PxFoundation * foundation_;
	PxPhysics * physics_;
	PxScene * scene_;
	PxCpuDispatcher * cpuDispatcher_;
	Physics();
public:
	~Physics();

	static Physics & getInstance();

	void step(double);
};
