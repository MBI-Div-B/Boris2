#include "Atom_DemagCUDA.h"

#if COMPILECUDA == 1

#if defined(MODULE_COMPILATION_DEMAG) && ATOMISTIC == 1

#include "BorisCUDALib.cuh"
#include "Atom_MeshCUDA.h"

//----------------------- Initialization

__global__ void set_Atom_DemagCUDA_pointers_kernel(
	ManagedAtom_MeshCUDA& cuaMesh, cuVEC<cuReal3>& Module_Heff)
{
	if (threadIdx.x == 0) cuaMesh.pAtom_Demag_Heff = &Module_Heff;
}

void Atom_DemagCUDA::set_Atom_DemagCUDA_pointers(void)
{
	set_Atom_DemagCUDA_pointers_kernel <<< 1, CUDATHREADS >>>
		(paMeshCUDA->cuaMesh, Module_Heff);
}

//----------------------- Auxiliary

__global__ void Atom_Demag_EvalSpeedup_SubSelf_Kernel(
	cuVEC<cuReal3>& H,
	cuVEC<cuReal3>& M, cuReal3& selfDemagCoeff)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < H.linear_size()) {

		H[idx] -= (selfDemagCoeff & M[idx]);
	}
}

//QUINTIC
__global__ void Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel(
	cuVEC<cuReal3>& H,
	cuVEC<cuReal3>& Hdemag, cuVEC<cuReal3>& Hdemag2, cuVEC<cuReal3>& Hdemag3, cuVEC<cuReal3>& Hdemag4, cuVEC<cuReal3>& Hdemag5, cuVEC<cuReal3>& Hdemag6,
	cuBReal a1, cuBReal a2, cuBReal a3, cuBReal a4, cuBReal a5, cuBReal a6,
	cuVEC<cuReal3>& M, cuReal3& selfDemagCoeff)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < H.linear_size()) {

		H[idx] = Hdemag[idx] * a1 + Hdemag2[idx] * a2 + Hdemag3[idx] * a3 + Hdemag4[idx] * a4 + Hdemag5[idx] * a5 + Hdemag6[idx] * a6 + (selfDemagCoeff & M[idx]);
	}
}

//QUARTIC
__global__ void Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel(
	cuVEC<cuReal3>& H,
	cuVEC<cuReal3>& Hdemag, cuVEC<cuReal3>& Hdemag2, cuVEC<cuReal3>& Hdemag3, cuVEC<cuReal3>& Hdemag4, cuVEC<cuReal3>& Hdemag5,
	cuBReal a1, cuBReal a2, cuBReal a3, cuBReal a4, cuBReal a5,
	cuVEC<cuReal3>& M, cuReal3& selfDemagCoeff)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < H.linear_size()) {

		H[idx] = Hdemag[idx] * a1 + Hdemag2[idx] * a2 + Hdemag3[idx] * a3 + Hdemag4[idx] * a4 + Hdemag5[idx] * a5 + (selfDemagCoeff & M[idx]);
	}
}

//CUBIC
__global__ void Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel(
	cuVEC<cuReal3>& H,
	cuVEC<cuReal3>& Hdemag, cuVEC<cuReal3>& Hdemag2, cuVEC<cuReal3>& Hdemag3, cuVEC<cuReal3>& Hdemag4,
	cuBReal a1, cuBReal a2, cuBReal a3, cuBReal a4,
	cuVEC<cuReal3>& M, cuReal3& selfDemagCoeff)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < H.linear_size()) {

		H[idx] = Hdemag[idx] * a1 + Hdemag2[idx] * a2 + Hdemag3[idx] * a3 + Hdemag4[idx] * a4 + (selfDemagCoeff & M[idx]);
	}
}

//QUADRATIC
__global__ void Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel(
	cuVEC<cuReal3>& H,
	cuVEC<cuReal3>& Hdemag, cuVEC<cuReal3>& Hdemag2, cuVEC<cuReal3>& Hdemag3,
	cuBReal a1, cuBReal a2, cuBReal a3,
	cuVEC<cuReal3>& M, cuReal3& selfDemagCoeff)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < H.linear_size()) {

		H[idx] = Hdemag[idx] * a1 + Hdemag2[idx] * a2 + Hdemag3[idx] * a3 + (selfDemagCoeff & M[idx]);
	}
}

//LINEAR
__global__ void Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel(
	cuVEC<cuReal3>& H,
	cuVEC<cuReal3>& Hdemag, cuVEC<cuReal3>& Hdemag2,
	cuBReal a1, cuBReal a2,
	cuVEC<cuReal3>& M, cuReal3& selfDemagCoeff)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < H.linear_size()) {

		H[idx] = Hdemag[idx] * a1 + Hdemag2[idx] * a2 + (selfDemagCoeff & M[idx]);
	}
}

//STEP
__global__ void Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel(
	cuVEC<cuReal3>& H,
	cuVEC<cuReal3>& Hdemag,
	cuVEC<cuReal3>& M, cuReal3& selfDemagCoeff)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < H.linear_size()) {

		H[idx] = Hdemag[idx] + (selfDemagCoeff & M[idx]);
	}
}

//----------------------- LAUNCHERS

void Atom_DemagCUDA::Atom_Demag_EvalSpeedup_SubSelf(cu_obj<cuVEC<cuReal3>>& H)
{
	Atom_Demag_EvalSpeedup_SubSelf_Kernel <<< (paMeshCUDA->n_dm.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >>>
		(H, M, selfDemagCoeff);
}

//QUINTIC
void Atom_DemagCUDA::Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf(cu_obj<cuVEC<cuReal3>>& H, cuBReal a1, cuBReal a2, cuBReal a3, cuBReal a4, cuBReal a5, cuBReal a6)
{
	Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel << < (paMeshCUDA->n_dm.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >> >
		(H, Hdemag, Hdemag2, Hdemag3, Hdemag4, Hdemag5, Hdemag6, a1, a2, a3, a4, a5, a6, M, selfDemagCoeff);
}

//QUARTIC
void Atom_DemagCUDA::Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf(cu_obj<cuVEC<cuReal3>>& H, cuBReal a1, cuBReal a2, cuBReal a3, cuBReal a4, cuBReal a5)
{
	Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel << < (paMeshCUDA->n_dm.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >> >
		(H, Hdemag, Hdemag2, Hdemag3, Hdemag4, Hdemag5, a1, a2, a3, a4, a5, M, selfDemagCoeff);
}

//CUBIC
void Atom_DemagCUDA::Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf(cu_obj<cuVEC<cuReal3>>& H, cuBReal a1, cuBReal a2, cuBReal a3, cuBReal a4)
{
	Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel << < (paMeshCUDA->n_dm.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >> >
		(H, Hdemag, Hdemag2, Hdemag3, Hdemag4, a1, a2, a3, a4, M, selfDemagCoeff);
}

//QUADRATIC
void Atom_DemagCUDA::Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf(cu_obj<cuVEC<cuReal3>>& H, cuBReal a1, cuBReal a2, cuBReal a3)
{
	Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel <<< (paMeshCUDA->n_dm.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >>>
		(H, Hdemag, Hdemag2, Hdemag3, a1, a2, a3, M, selfDemagCoeff);
}

//LINEAR
void Atom_DemagCUDA::Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf(cu_obj<cuVEC<cuReal3>>& H, cuBReal a1, cuBReal a2)
{
	Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel <<< (paMeshCUDA->n_dm.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >>>
		(H, Hdemag, Hdemag2, a1, a2, M, selfDemagCoeff);
}

//STEP
void Atom_DemagCUDA::Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf(cu_obj<cuVEC<cuReal3>>& H)
{
	Atom_Demag_EvalSpeedup_SetExtrapField_AddSelf_Kernel <<< (paMeshCUDA->n_dm.dim() + CUDATHREADS) / CUDATHREADS, CUDATHREADS >>>
		(H, Hdemag, M, selfDemagCoeff);
}

#endif

#endif