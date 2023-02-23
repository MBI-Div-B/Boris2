#pragma once

#include "Boris_Enums_Defs.h"
#if COMPILECUDA == 1

#include "BorisCUDALib.h"

#include "ConvolutionDataCUDA.h"

#include "ErrorHandler.h"

template <typename Owner, typename Kernel>
class ConvolutionCUDA :
	public virtual ConvolutionDataCUDA,		//virtual to solve the diamond problem as it's also inherited by Kernel
	public Kernel
{

private:

	//if the object couldn't be created properly in the constructor an error is set here
	BError convolution_error_on_create;

private:

	//Embedded (default)

	//convolute In with kernels, set output in Out. 2D is for n.z == 1.
	//set energy value : product of In with Out times -MU0 / (2 * non_empty_points), where non_empty_points = In.get_nonempty_points();
	//If clearOut flag is true then Out is set, otherwise Out is added into.
	//SINGLE INPUT, SINGLE OUTPUT
	template <typename cuVECIn, typename cuVECOut>
	void Convolute_2D(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);
	
	template <typename cuVECIn, typename cuVECOut>
	void Convolute_3D(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//AVERAGED INPUTS, SINGLE OUTPUT

	//Same as Convolution with (In1 + In2) / 2 as input.
	template <typename cuVECIn, typename cuVECOut>
	void Convolute_2D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	template <typename cuVECIn, typename cuVECOut>
	void Convolute_3D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//AVERAGED INPUTS, DUPLICATED OUTPUTS
	//Same as Convolution with (In1 + In2) / 2 as input and output copied to both Out1 and Out2.
	template <typename cuVECIn, typename cuVECOut>
	void Convolute_2D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	template <typename cuVECIn, typename cuVECOut>
	void Convolute_3D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//Not embedded

	//Forward FFT. In -> cuS

	//SINGLE INPUT

	template <typename cuVECIn>
	void ForwardFFT_2D(cu_obj<cuVECIn>& In);

	template <typename cuVECIn>
	void ForwardFFT_3D(cu_obj<cuVECIn>& In);

	//AVERAGED INPUTS

	template <typename cuVECIn>
	void ForwardFFT_2D(cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2);

	template <typename cuVECIn>
	void ForwardFFT_3D(cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2);

	//Inverse FFT. cuS2 -> Out

	//SINGLE INPUT, SINGLE OUTPUT

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_2D(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_3D(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_2D(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_3D(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//AVERAGED INPUTS, SINGLE OUTPUT

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_2D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_3D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_2D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_3D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//AVERAGED INPUTS, DUPLICATED OUTPUTS

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_2D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_3D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_2D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_3D(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr);

protected:

	//-------------------------- CONSTRUCTORS

	ConvolutionCUDA(void) :
		ConvolutionDataCUDA(),
		Kernel()
	{}

	ConvolutionCUDA(cuSZ3 n_, cuReal3 h_);

	virtual ~ConvolutionCUDA() {}

	//-------------------------- ERROR CHECKER

	BError Convolution_Error_on_Create(void) { return convolution_error_on_create; }

	//-------------------------- CONFIGURATION

	//This method sets all values from n and h, including allocating memory - call this before initializing kernels or doing any convolutions
	BError SetDimensions(cuSZ3 n_, cuReal3 h_, bool multiplication_embedding_ = true, cuINT3 pbc_images_ = cuINT3());

	//-------------------------- CHECK

	//return true only if both n_ and h_ match the current dimensions (n and h); also number of pbc images must match
	bool CheckDimensions(cuSZ3 n_, cuReal3 h_, cuINT3 pbc_images_) { return (n == n_ && h == h_ && pbc_images == pbc_images_); }

	//-------------------------- RUN-TIME CONVOLUTION

	//SINGLE INPUT, SINGLE OUTPUT

	//Run a 2D or 3D convolution depending on set n (n.z == 1 for 2D)
	template <typename cuVECIn, typename cuVECOut>
	void Convolute(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) Convolute_2D(In, Out, energy, get_energy, clearOut, pH, penergy);
		else Convolute_3D(In, Out, energy, get_energy, clearOut, pH, penergy);
	}

	//AVERAGED INPUTS, SINGLE OUTPUT

	//Same as Convolution with (In1 + In2) / 2 as input.
	template <typename cuVECIn, typename cuVECOut>
	void Convolute_AveragedInputs(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) Convolute_2D(In1, In2, Out, energy, get_energy, clearOut, pH, penergy);
		else Convolute_3D(In1, In2, Out, energy, get_energy, clearOut, pH, penergy);
	}

	//AVERAGED INPUTS, DUPLICATED OUTPUTS

	//Same as Convolution with (In1 + In2) / 2 as input and output copied to both Out1 and Out2.
	template <typename cuVECIn, typename cuVECOut>
	void Convolute_AveragedInputs_DuplicatedOutputs(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) Convolute_2D(In1, In2, Out1, Out2, energy, get_energy, clearOut, pH, penergy);
		else Convolute_3D(In1, In2, Out1, Out2, energy, get_energy, clearOut, pH, penergy);
	}

	//Convolution broken down into parts : forward FFT, Kernel Multiplication, inverse FFT

	//SINGLE INPUT

	template <typename cuVECIn>
	void ForwardFFT(cu_obj<cuVECIn>& In)
	{
		if (n.z == 1) ForwardFFT_2D(In);
		else ForwardFFT_3D(In);
	}

	//AVERAGED INPUTS

	template <typename cuVECIn>
	void ForwardFFT_AveragedInputs(cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2)
	{
		if (n.z == 1) ForwardFFT_2D(In1, In2);
		else ForwardFFT_3D(In1, In2);
	}

	//MULTIPLICATION

	//2. (S -> S2) -> multiple input spaces version using a collection of FFT spaces (Kernel must be configured for this)
	void KernelMultiplication_MultipleInputs(std::vector<cu_arr<cuBComplex>*>& Scol_x, std::vector<cu_arr<cuBComplex>*>& Scol_y, std::vector<cu_arr<cuBComplex>*>& Scol_z)
	{
		if (n.z == 1) static_cast<Owner*>(this)->KernelMultiplication_2D(Scol_x, Scol_y, Scol_z, cuS2_x, cuS2_y, cuS2_z);
		else static_cast<Owner*>(this)->KernelMultiplication_3D(Scol_x, Scol_y, Scol_z, cuS2_x, cuS2_y, cuS2_z);
	}

	//SINGLE INPUT, SINGLE OUTPUT

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) InverseFFT_2D(In, Out, energy, get_energy, clearOut, pH, penergy);
		else InverseFFT_3D(In, Out, energy, get_energy, clearOut, pH, penergy);
	}

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT(
		cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) InverseFFT_2D(In, Out, energy, energy_weight, clearOut, pH, penergy);
		else InverseFFT_3D(In, Out, energy, energy_weight, clearOut, pH, penergy);
	}

	//AVERAGED INPUTS, SINGLE OUTPUT

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_AveragedInputs(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) InverseFFT_2D(In1, In2, Out, energy, get_energy, clearOut, pH, penergy);
		else InverseFFT_3D(In1, In2, Out, energy, get_energy, clearOut, pH, penergy);
	}

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_AveragedInputs(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) InverseFFT_2D(In1, In2, Out, energy, energy_weight, clearOut, pH, penergy);
		else InverseFFT_3D(In1, In2, Out, energy, energy_weight, clearOut, pH, penergy);
	}

	//AVERAGED INPUTS, DUPLICATED OUTPUTS

	//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_AveragedInputs_DuplicatedOutputs(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
		cu_obj<cuBReal>& energy, bool get_energy, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) InverseFFT_2D(In1, In2, Out1, Out2, energy, get_energy, clearOut, pH, penergy);
		else InverseFFT_3D(In1, In2, Out1, Out2, energy, get_energy, clearOut, pH, penergy);
	}

	//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
	//This is useful when calculating an energy total from multiple meshes which might need different weights.
	template <typename cuVECIn, typename cuVECOut>
	void InverseFFT_AveragedInputs_DuplicatedOutputs(
		cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
		cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut = true, 
		cu_obj<cuVEC<cuReal3>>* pH = nullptr, cu_obj<cuVEC<cuBReal>>* penergy = nullptr)
	{
		if (n.z == 1) InverseFFT_2D(In1, In2, Out1, Out2, energy, energy_weight, clearOut, pH, penergy);
		else InverseFFT_3D(In1, In2, Out1, Out2, energy, energy_weight, clearOut, pH, penergy);
	}
};

//-------------------------- CONSTRUCTORS

template <typename Owner, typename Kernel>
ConvolutionCUDA<Owner, Kernel>::ConvolutionCUDA(cuSZ3 n_, cuReal3 h_) :
	ConvolutionDataCUDA(),
	Kernel()
{
	convolution_error_on_create = SetDimensions(n_, h_);
}

//-------------------------- CONFIGURATION

template <typename Owner, typename Kernel>
BError ConvolutionCUDA<Owner, Kernel>::SetDimensions(cuSZ3 n_, cuReal3 h_, bool multiplication_embedding_, cuINT3 pbc_images_)
{
	BError error(__FUNCTION__);

	error = SetConvolutionDimensions(n_, h_, multiplication_embedding_, pbc_images_);
	if (!error) error = static_cast<Owner*>(this)->AllocateKernelMemory();

	return error;
}

//-------------------------- RUN-TIME CONVOLUTION : 2D (embedded)

//SINGLE INPUT, SINGLE OUTPUT

template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::Convolute_2D(
	cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Copy In to cufft arrays, setting all other points to zero
	CopyInputData(In);

	//Forward 2D FFT
	forward_fft_2D();

	//Multiplication with kernels
	static_cast<Owner*>(this)->KernelMultiplication_2D();

	//Inverse 2D FFT
	inverse_fft_2D();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In, Out, energy, get_energy, pH, penergy);
	}
}

//AVERAGED INPUTS, SINGLE OUTPUT

template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::Convolute_2D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Copy In to cufft arrays, setting all other points to zero
	AverageInputData(In1, In2);

	//Forward 2D FFT
	forward_fft_2D();

	//Multiplication with kernels
	static_cast<Owner*>(this)->KernelMultiplication_2D();

	//Inverse 2D FFT
	inverse_fft_2D();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out, energy, get_energy, pH, penergy);
	}
}

//AVERAGED INPUTS, DUPLICATED OUTPUTS

template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::Convolute_2D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Copy In to cufft arrays, setting all other points to zero
	AverageInputData(In1, In2);

	//Forward 2D FFT
	forward_fft_2D();

	//Multiplication with kernels
	static_cast<Owner*>(this)->KernelMultiplication_2D();

	//Inverse 2D FFT
	inverse_fft_2D();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
}

//-------------------------- RUN-TIME CONVOLUTION : 3D (embedded)

//SINGLE INPUT, SINGLE OUTPUT

template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::Convolute_3D(
	cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Copy In to cufft arrays, setting all other points to zero
	CopyInputData(In);

	if (!q2D_level) {

		//Forward 3D FFT
		forward_fft_3D();

		//Multiplication with kernels
		static_cast<Owner*>(this)->KernelMultiplication_3D();

		//Inverse 3D FFT
		inverse_fft_3D();
	}
	else {

		//Forward q2D FFT
		forward_fft_q2D();

		//Multiplication with kernels with z-axis fft and ifft rolled into one step
		static_cast<Owner*>(this)->KernelMultiplication_q2D(q2D_level);

		//Inverse q2D FFT
		inverse_fft_q2D();
	}

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In, Out, energy, get_energy, pH, penergy);
	}
}

//AVERAGED INPUTS, SINGLE OUTPUT

template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::Convolute_3D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Copy In to cufft arrays, setting all other points to zero
	AverageInputData(In1, In2);

	if (!q2D_level) {

		//Forward 3D FFT
		forward_fft_3D();

		//Multiplication with kernels
		static_cast<Owner*>(this)->KernelMultiplication_3D();

		//Inverse 3D FFT
		inverse_fft_3D();
	}
	else {

		//Forward q2D FFT
		forward_fft_q2D();

		//Multiplication with kernels with z-axis fft and ifft rolled into one step
		static_cast<Owner*>(this)->KernelMultiplication_q2D(q2D_level);

		//Inverse q2D FFT
		inverse_fft_q2D();
	}

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out, energy, get_energy, pH, penergy);
	}
}

//AVERAGED INPUTS, DUPLICATED OUTPUTS

template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::Convolute_3D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Copy In to cufft arrays, setting all other points to zero
	AverageInputData(In1, In2);

	if (!q2D_level) {

		//Forward 3D FFT
		forward_fft_3D();

		//Multiplication with kernels
		static_cast<Owner*>(this)->KernelMultiplication_3D();

		//Inverse 3D FFT
		inverse_fft_3D();
	}
	else {

		//Forward q2D FFT
		forward_fft_q2D();

		//Multiplication with kernels with z-axis fft and ifft rolled into one step
		static_cast<Owner*>(this)->KernelMultiplication_q2D(q2D_level);

		//Inverse q2D FFT
		inverse_fft_q2D();
	}

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
}

//-------------------------- RUN-TIME CONVOLUTION : 2D (not embedded)

//SINGLE INPUT

template <typename Owner, typename Kernel>
template <typename cuVECIn>
void ConvolutionCUDA<Owner, Kernel>::ForwardFFT_2D(cu_obj<cuVECIn>& In)
{
	//Copy In to cufft arrays, setting all other points to zero
	CopyInputData(In);

	//Forward 2D FFT
	forward_fft_2D();
}

//AVERAGED INPUTS

template <typename Owner, typename Kernel>
template <typename cuVECIn>
void ConvolutionCUDA<Owner, Kernel>::ForwardFFT_2D(cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2)
{
	//Copy In to cufft arrays, setting all other points to zero
	AverageInputData(In1, In2);

	//Forward 2D FFT
	forward_fft_2D();
}

//SINGLE INPUT, SINGLE OUTPUT

//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_2D(
	cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
	cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 2D FFT
	inverse_fft_2D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In, Out, energy, get_energy, pH, penergy);
	}
}

//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
//This is useful when calculating an energy total from multiple meshes which might need different weights.
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_2D(
	cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, cu_obj<cuBReal>& energy, 
	cu_obj<cuBReal>& energy_weight, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 2D FFT
	inverse_fft_2D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In, Out, energy, energy_weight, pH, penergy);
	}
	else {

		FinishConvolution_Add(In, Out, energy, energy_weight, pH, penergy);
	}
}

//AVERAGED INPUTS, SINGLE OUTPUT

//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_2D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
	cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 2D FFT
	inverse_fft_2D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out, energy, get_energy, pH, penergy);
	}
}

//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
//This is useful when calculating an energy total from multiple meshes which might need different weights.
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_2D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
	cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 2D FFT
	inverse_fft_2D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out, energy, energy_weight, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out, energy, energy_weight, pH, penergy);
	}
}

//AVERAGED INPUTS, DUPLICATED OUTPUTS

//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_2D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
	cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 2D FFT
	inverse_fft_2D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
}

//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
//This is useful when calculating an energy total from multiple meshes which might need different weights.
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_2D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
	cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 2D FFT
	inverse_fft_2D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out1, Out2, energy, energy_weight, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out1, Out2, energy, energy_weight, pH, penergy);
	}
}

//-------------------------- RUN-TIME CONVOLUTION : 3D (not embedded)

//SINGLE INPUT

template <typename Owner, typename Kernel>
template <typename cuVECIn>
void ConvolutionCUDA<Owner, Kernel>::ForwardFFT_3D(cu_obj<cuVECIn>& In)
{
	//Copy In to cufft arrays, setting all other points to zero
	CopyInputData(In);

	//Forward 3D FFT
	forward_fft_3D();
}

//AVERAGED INPUTS

template <typename Owner, typename Kernel>
template <typename cuVECIn>
void ConvolutionCUDA<Owner, Kernel>::ForwardFFT_3D(cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2)
{
	//Copy In to cufft arrays, setting all other points to zero
	AverageInputData(In1, In2);

	//Forward 3D FFT
	forward_fft_3D();
}

//SINGLE INPUT, SINGLE OUTPUT

//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_3D(
	cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
	cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 3D FFT
	inverse_fft_3D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In, Out, energy, get_energy, pH, penergy);
	}
}


//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
//This is useful when calculating an energy total from multiple meshes which might need different weights.
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_3D(
	cu_obj<cuVECIn>& In, cu_obj<cuVECOut>& Out, 
	cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 3D FFT
	inverse_fft_3D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In, Out, energy, energy_weight, pH, penergy);
	}
	else {

		FinishConvolution_Add(In, Out, energy, energy_weight, pH, penergy);
	}
}

//AVERAGED INPUTS, SINGLE OUTPUT

//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_3D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
	cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 3D FFT
	inverse_fft_3D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out, energy, get_energy, pH, penergy);
	}
}


//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
//This is useful when calculating an energy total from multiple meshes which might need different weights.
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_3D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out, 
	cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 3D FFT
	inverse_fft_3D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out, energy, energy_weight, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out, energy, energy_weight, pH, penergy);
	}
}

//AVERAGED INPUTS, DUPLICATED OUTPUTS

//inverse FFT with the option of calculating an energy contribution ((-MU0/2)(In * Out) / input_non_empty_cells
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_3D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
	cu_obj<cuBReal>& energy, bool get_energy, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 3D FFT
	inverse_fft_3D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out1, Out2, energy, get_energy, pH, penergy);
	}
}


//inverse FFT with calculation of a weighted energy contribution: weight*((-MU0/2)(In * Out) / input_non_empty_cells.
//This is useful when calculating an energy total from multiple meshes which might need different weights.
template <typename Owner, typename Kernel>
template <typename cuVECIn, typename cuVECOut>
void ConvolutionCUDA<Owner, Kernel>::InverseFFT_3D(
	cu_obj<cuVECIn>& In1, cu_obj<cuVECIn>& In2, cu_obj<cuVECOut>& Out1, cu_obj<cuVECOut>& Out2, 
	cu_obj<cuBReal>& energy, cu_obj<cuBReal>& energy_weight, bool clearOut,
	cu_obj<cuVEC<cuReal3>>* pH, cu_obj<cuVEC<cuBReal>>* penergy)
{
	//Inverse 3D FFT
	inverse_fft_3D_2();

	//Copy cufft arrays to Heff
	if (clearOut) {

		FinishConvolution_Set(In1, In2, Out1, Out2, energy, energy_weight, pH, penergy);
	}
	else {

		FinishConvolution_Add(In1, In2, Out1, Out2, energy, energy_weight, pH, penergy);
	}
}

#endif