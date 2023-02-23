#include "DiffEqFMCUDA.h"

#if COMPILECUDA == 1
#ifdef MESH_COMPILATION_FERROMAGNETIC

#include "BorisCUDALib.h"
#include "BorisCUDALib.cuh"

#include "MeshParamsControlCUDA.h"

//---------------------------------------- OTHER CALCULATION METHODS : GENERATE THERMAL cuVECs

//----------------------------------------

__global__ void GenerateThermalField_Kernel(cuBorisRand<>& prng, ManagedDiffEqFMCUDA& cuDiffEq, ManagedMeshCUDA& cuMesh, cuBReal& deltaT)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	cuBReal grel = cuMesh.pgrel->get0();

	if (idx < cuDiffEq.pH_Thermal->linear_size() && cuIsNZ(grel)) {

		cuReal3 position = cuDiffEq.pH_Thermal->cellidx_to_position(idx);

		if (cuMesh.pM->is_not_empty(position) && !cuMesh.pM->is_skipcell(position)) {

			cuReal3 h = cuDiffEq.pH_Thermal->h;

			cuBReal Temperature;

			if (cuMesh.pTemp->linear_size()) {

				//get temperature at centre of idx M cell
				Temperature = (*cuMesh.pTemp)[position];
			}
			else Temperature = (*cuMesh.pbase_temperature);

			cuBReal s_eff = *cuMesh.ps_eff;
			cuMesh.update_parameters_atposition(position, *cuMesh.ps_eff, s_eff);

			//do not include any damping here - this will be included in the stochastic equations
			cuBReal Hth_const = s_eff * sqrt(2 * (cuBReal)BOLTZMANN * Temperature / ((cuBReal)GAMMA * grel * h.dim() * (cuBReal)MU0 * cuMesh.pMs->get0() * deltaT));
			
			(*cuDiffEq.pH_Thermal)[idx] = Hth_const * cuReal3(prng.rand_gauss(0, 1), prng.rand_gauss(0, 1), prng.rand_gauss(0, 1));
		}
	}
}

//called when using stochastic equations
void DifferentialEquationFMCUDA::GenerateThermalField_CUDA(cu_obj<cuBReal>& deltaT)
{
	GenerateThermalField_Kernel <<< (pMeshCUDA->n_s.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >>> (prng, cuDiffEq, pMeshCUDA->cuMesh, deltaT);
}

//----------------------------------------

__global__ void GenerateThermalField_and_Torque_Kernel(cuBorisRand<>& prng, ManagedDiffEqFMCUDA& cuDiffEq, ManagedMeshCUDA& cuMesh, cuBReal& deltaT)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	cuBReal grel = cuMesh.pgrel->get0();

	if (idx < cuDiffEq.pH_Thermal->linear_size() && cuIsNZ(grel)) {

		cuReal3 position = cuDiffEq.pH_Thermal->cellidx_to_position(idx);

		if (cuMesh.pM->is_not_empty(position) && !cuMesh.pM->is_skipcell(position)) {

			cuReal3 h = cuDiffEq.pH_Thermal->h;

			cuBReal Temperature;

			if (cuMesh.pTemp->linear_size()) {
				
				//get temperature at centre of idx M cell
				Temperature = (*cuMesh.pTemp)[position];
			}
			else Temperature = (*cuMesh.pbase_temperature);

			cuBReal s_eff = *cuMesh.ps_eff;
			cuMesh.update_parameters_atposition(position, *cuMesh.ps_eff, s_eff);
			
			//1. Thermal Field
			//do not include any damping here - this will be included in the stochastic equations
			cuBReal Hth_const = s_eff * sqrt(2 * (cuBReal)BOLTZMANN * Temperature / ((cuBReal)GAMMA * grel * h.dim() * (cuBReal)MU0 * cuMesh.pMs->get0() * deltaT));

			(*cuDiffEq.pH_Thermal)[idx] = Hth_const * cuReal3(prng.rand_gauss(0, 1), prng.rand_gauss(0, 1), prng.rand_gauss(0, 1));
			
			//2. Thermal Torque
			//do not include any damping here - this will be included in the stochastic equations
			cuBReal Tth_const = s_eff * sqrt(2 * (cuBReal)BOLTZMANN * Temperature * (cuBReal)GAMMA * grel * cuMesh.pMs->get0() / ((cuBReal)MU0 * h.dim() * deltaT));
			
			(*cuDiffEq.pTorque_Thermal)[idx] = Tth_const * cuReal3(prng.rand_gauss(0, 1), prng.rand_gauss(0, 1), prng.rand_gauss(0, 1));
		}
	}
}

void DifferentialEquationFMCUDA::GenerateThermalField_and_Torque_CUDA(cu_obj<cuBReal>& deltaT)
{
	GenerateThermalField_and_Torque_Kernel <<< (pMeshCUDA->n_s.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >>> (prng, cuDiffEq, pMeshCUDA->cuMesh, deltaT);
}

#endif
#endif