#include "TMRCUDA.h"

#if COMPILECUDA == 1

#ifdef MODULE_COMPILATION_TMR

#include "MeshCUDA.h"

#include "BorisCUDALib.cuh"

//-------------------Calculation Methods

void TMRCUDA::IterateChargeSolver_SOR(cu_obj<cuBReal>& damping, cu_obj<cuBReal>& max_error, cu_obj<cuBReal>& max_value)
{
	pMeshCUDA->V()->IteratePoisson_SOR(pMeshCUDA->n_e.dim(), (TransportCUDA_V_Funcs&)poisson_V, damping, max_error, max_value);
}

#endif

#endif