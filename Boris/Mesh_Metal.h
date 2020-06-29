#pragma once

#include "Mesh.h"

#if COMPILECUDA == 1
#include "Mesh_MetalCUDA.h"
#endif

#ifdef MESH_COMPILATION_METAL

#include "Transport.h"
#include "Heat.h"

/////////////////////////////////////////////////////////////////////
//
//Metallic Material Mesh

class SuperMesh;

class MetalMesh :
	public Mesh,
	public ProgramState<MetalMesh,
	tuple<
	//Mesh members
	int, int, int, 
	int, int, 
	int, int, 
	Rect, SZ3, DBL3, SZ3, DBL3, SZ3, DBL3, 
	VEC_VC<double>, VEC_VC<DBL3>, VEC_VC<double>, VEC_VC<double>, VEC_VC<double>, 
	vector_lut<Modules*>,
	//Members in this derived class
	
	//Material Parameters
	MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<DBL2, double>, MatP<DBL2, double>, double, TEquation<double>, MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<double, double>, MatP<double, double>
	>,
	//Module Implementations
	tuple<Transport, Heat> >
{

#if COMPILECUDA == 1
	friend MetalMeshCUDA;
#endif

public:

	//constructor taking only a SuperMesh pointer (SuperMesh is the owner) only needed for loading : all required values will be set by LoadObjectState method in ProgramState
	MetalMesh(SuperMesh *pSMesh_);

	MetalMesh(Rect meshRect_, DBL3 h_, SuperMesh *pSMesh_);

	~MetalMesh() {}

	//implement pure virtual method from ProgramState
	void RepairObjectState(void);

	//----------------------------------- INITIALIZATION

	//----------------------------------- IMPORTANT CONTROL METHODS

	//call when a configuration change has occurred - some objects might need to be updated accordingly
	BError UpdateConfiguration(UPDATECONFIG_ cfgMessage);
	void UpdateConfiguration_Values(UPDATECONFIG_ cfgMessage);

	BError SwitchCUDAState(bool cudaState);

	//called at the start of each iteration
	void PrepareNewIteration(void) {}

#if COMPILECUDA == 1
	void PrepareNewIterationCUDA(void) {}
#endif

	//Check if mesh needs to be moved (using the MoveMesh method) - return amount of movement required (i.e. parameter to use when calling MoveMesh).
	double CheckMoveMesh(void) { return 0.0; }
};

#else

class MetalMesh :
	public Mesh
{

public:

	//constructor taking only a SuperMesh pointer (SuperMesh is the owner) only needed for loading : all required values will be set by LoadObjectState method in ProgramState
	MetalMesh(SuperMesh *pSMesh_) :
		Mesh(MESH_METAL, pSMesh_)
	{}

	MetalMesh(Rect meshRect_, DBL3 h_, SuperMesh *pSMesh_) :
		Mesh(MESH_METAL, pSMesh_)
	{}

	~MetalMesh() {}

	//----------------------------------- INITIALIZATION

	//----------------------------------- IMPORTANT CONTROL METHODS

	//call when a configuration change has occurred - some objects might need to be updated accordingly
	BError UpdateConfiguration(UPDATECONFIG_ cfgMessage) { return BError(); }
	void UpdateConfiguration_Values(UPDATECONFIG_ cfgMessage) {}

	BError SwitchCUDAState(bool cudaState) { return BError(); }

	//called at the start of each iteration
	void PrepareNewIteration(void) {}

#if COMPILECUDA == 1
	void PrepareNewIterationCUDA(void) {}
#endif

	//Check if mesh needs to be moved (using the MoveMesh method) - return amount of movement required (i.e. parameter to use when calling MoveMesh).
	double CheckMoveMesh(void) { return 0.0; }
};

#endif
