#include "stdafx.h"
#include "Atom_DMExchange.h"

#if defined(MODULE_DMEXCHANGE) && ATOMISTIC == 1

#include "Atom_Mesh.h"
#include "Atom_MeshParamsControl.h"

#if COMPILECUDA == 1
#include "Atom_DMExchangeCUDA.h"
#endif

Atom_DMExchange::Atom_DMExchange(Atom_Mesh *paMesh_) :
	Modules(),
	ProgramStateNames(this, {}, {})
{
	paMesh = paMesh_;

	error_on_create = UpdateConfiguration(UPDATECONFIG_FORCEUPDATE);

	//-------------------------- Is CUDA currently enabled?

	//If cuda is enabled we also need to make the cuda module version
	if (paMesh->cudaEnabled) {

		if (!error_on_create) error_on_create = SwitchCUDAState(true);
	}
}

BError Atom_DMExchange::Initialize(void)
{
	BError error(CLASS_STR(Atom_DMExchange));

	initialized = true;

	non_empty_volume = paMesh->Get_NonEmpty_Magnetic_Volume();

	return error;
}

BError Atom_DMExchange::UpdateConfiguration(UPDATECONFIG_ cfgMessage)
{
	BError error(CLASS_STR(Atom_DMExchange));

	Uninitialize();

	Initialize();

	//------------------------ CUDA UpdateConfiguration if set

#if COMPILECUDA == 1
	if (pModuleCUDA) {

		if (!error) error = pModuleCUDA->UpdateConfiguration(cfgMessage);
	}
#endif

	return error;
}

BError Atom_DMExchange::MakeCUDAModule(void)
{
	BError error(CLASS_STR(Atom_DMExchange));

#if COMPILECUDA == 1

	if (paMesh->paMeshCUDA) {

		//Note : it is posible pMeshCUDA has not been allocated yet, but this module has been created whilst cuda is switched on. This will happen when a new mesh is being made which adds this module by default.
		//In this case, after the mesh has been fully made, it will call SwitchCUDAState on the mesh, which in turn will call this SwitchCUDAState method; then pMeshCUDA will not be nullptr and we can make the cuda module version
		pModuleCUDA = new Atom_DMExchangeCUDA(paMesh->paMeshCUDA);
		error = pModuleCUDA->Error_On_Create();
	}

#endif

	return error;
}

double Atom_DMExchange::UpdateField(void)
{
	double energy = 0;

#pragma omp parallel for reduction(+:energy)
	for (int idx = 0; idx < paMesh->n.dim(); idx++) {

		if (paMesh->M1.is_not_empty(idx)) {

			double mu_s = paMesh->mu_s;
			double J = paMesh->J;
			double D = paMesh->D;
			paMesh->update_parameters_mcoarse(idx, paMesh->mu_s, mu_s, paMesh->J, J, paMesh->D, D);

			//update effective field with the Heisenberg and DMI exchange field
			DBL3 Heff_value = (J * paMesh->M1.ngbr_sum(idx) + D * paMesh->M1.anisotropic_ngbr_sum(idx)) / (MUB_MU0*mu_s*mu_s);

			paMesh->Heff1[idx] += Heff_value;

			//update energy E = -mu_s * Bex
			energy -= MUB_MU0 * paMesh->M1[idx] * Heff_value;
		}
	}

	//convert to energy density and return
	if (non_empty_volume) this->energy = energy / non_empty_volume;
	else this->energy = 0.0;

	return this->energy;
}

//-------------------Energy density methods

double Atom_DMExchange::GetEnergyDensity(Rect& avRect)
{
#if COMPILECUDA == 1
	if (pModuleCUDA) return dynamic_cast<Atom_DMExchangeCUDA*>(pModuleCUDA)->GetEnergyDensity(avRect);
#endif

	double energy = 0;

	int num_points = 0;

#pragma omp parallel for reduction(+:energy, num_points)
	for (int idx = 0; idx < paMesh->n.dim(); idx++) {

		//only average over values in given rectangle
		if (!avRect.contains(paMesh->M1.cellidx_to_position(idx))) continue;

		if (paMesh->M1.is_not_empty(idx)) {

			double mu_s = paMesh->mu_s;
			double J = paMesh->J;
			double D = paMesh->D;
			paMesh->update_parameters_mcoarse(idx, paMesh->mu_s, mu_s, paMesh->J, J, paMesh->D, D);

			//update effective field with the Heisenberg and DMI exchange field
			DBL3 Heff_value = (J * paMesh->M1.ngbr_sum(idx) + D * paMesh->M1.anisotropic_ngbr_sum(idx)) / (MUB_MU0*mu_s*mu_s);

			//update energy E = -mu_s * Bex
			energy -= MUB_MU0 * paMesh->M1[idx] * Heff_value;
			num_points++;
		}
	}

	//convert to energy density and return
	if (num_points) energy = energy / (num_points * paMesh->M1.h.dim());
	else energy = 0.0;

	return energy;
}

double Atom_DMExchange::GetEnergy_Max(Rect& rectangle)
{
#if COMPILECUDA == 1
	if (pModuleCUDA) return dynamic_cast<Atom_DMExchangeCUDA*>(pModuleCUDA)->GetEnergy_Max(rectangle);
#endif

	INT3 n = paMesh->n;

	OmpReduction<double> emax;
	emax.new_minmax_reduction();

#pragma omp parallel for
	for (int idx = 0; idx < n.dim(); idx++) {

		//only obtain max in given rectangle
		if (!rectangle.contains(paMesh->M1.cellidx_to_position(idx))) continue;

		if (paMesh->M1.is_not_empty(idx)) {

			double mu_s = paMesh->mu_s;
			double J = paMesh->J;
			double D = paMesh->D;
			paMesh->update_parameters_mcoarse(idx, paMesh->mu_s, mu_s, paMesh->J, J, paMesh->D, D);

			//update effective field with the Heisenberg and DMI exchange field
			DBL3 Heff_value = (J * paMesh->M1.ngbr_sum(idx) + D * paMesh->M1.anisotropic_ngbr_sum(idx)) / (MUB_MU0*mu_s*mu_s);

			//update energy E = -mu_s * Bex
			energy = -MUB_MU0 * paMesh->M1[idx] * Heff_value;

			emax.reduce_max(fabs(energy));
		}
	}

	//return maximum energy density modulus
	return emax.maximum() / paMesh->M1.h.dim();
}

//Compute exchange energy density and store it in displayVEC
void Atom_DMExchange::Compute_Exchange(VEC<double>& displayVEC)
{
	displayVEC.resize(paMesh->h, paMesh->meshRect);

#if COMPILECUDA == 1
	if (pModuleCUDA) {

		dynamic_cast<Atom_DMExchangeCUDA*>(pModuleCUDA)->Compute_Exchange(displayVEC);
		return;
	}
#endif

#pragma omp parallel for
	for (int idx = 0; idx < paMesh->n.dim(); idx++) {

		if (paMesh->M1.is_not_empty(idx)) {

			double mu_s = paMesh->mu_s;
			double J = paMesh->J;
			double D = paMesh->D;
			paMesh->update_parameters_mcoarse(idx, paMesh->mu_s, mu_s, paMesh->J, J, paMesh->D, D);

			//update effective field with the Heisenberg and DMI exchange field
			DBL3 Heff_value = (J * paMesh->M1.ngbr_sum(idx) + D * paMesh->M1.anisotropic_ngbr_sum(idx)) / (MUB_MU0*mu_s*mu_s);

			displayVEC[idx] = -MUB_MU0 * paMesh->M1[idx] * Heff_value / paMesh->M1.h.dim();
		}
		else displayVEC[idx] = 0.0;
	}
}

#endif