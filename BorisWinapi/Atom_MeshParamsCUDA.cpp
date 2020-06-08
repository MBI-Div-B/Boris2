#include "stdafx.h"
#include "Atom_MeshParamsCUDA.h"
#include "Atom_MeshParams.h"

#if COMPILECUDA == 1

Atom_MeshParamsCUDA::Atom_MeshParamsCUDA(Atom_MeshParams *pameshParams)
{
	this->pameshParams = pameshParams;

	//-----------SIMPLE CUBIC

	alpha()->set_from_cpu(pameshParams->alpha);
	pameshParams->alpha.set_p_cu_obj_mpcuda(&alpha);

	mu_s()->set_from_cpu(pameshParams->mu_s);
	pameshParams->mu_s.set_p_cu_obj_mpcuda(&mu_s);

	J()->set_from_cpu(pameshParams->J);
	pameshParams->J.set_p_cu_obj_mpcuda(&J);

	K()->set_from_cpu(pameshParams->K);
	pameshParams->K.set_p_cu_obj_mpcuda(&K);
	mcanis_ea1()->set_from_cpu(pameshParams->mcanis_ea1);
	pameshParams->mcanis_ea1.set_p_cu_obj_mpcuda(&mcanis_ea1);

	//-----------Others

	cHA()->set_from_cpu(pameshParams->cHA);
	pameshParams->cHA.set_p_cu_obj_mpcuda(&cHA);

	elecCond()->set_from_cpu(pameshParams->elecCond);
	pameshParams->elecCond.set_p_cu_obj_mpcuda(&elecCond);

	thermCond()->set_from_cpu(pameshParams->thermCond);
	pameshParams->thermCond.set_p_cu_obj_mpcuda(&thermCond);

	base_temperature.from_cpu(pameshParams->base_temperature);
	
	density()->set_from_cpu(pameshParams->density);
	pameshParams->density.set_p_cu_obj_mpcuda(&density);

	shc()->set_from_cpu(pameshParams->shc);
	pameshParams->shc.set_p_cu_obj_mpcuda(&shc);
	shc_e()->set_from_cpu(pameshParams->shc_e);
	pameshParams->shc_e.set_p_cu_obj_mpcuda(&shc_e);
	G_e()->set_from_cpu(pameshParams->G_e);
	pameshParams->G_e.set_p_cu_obj_mpcuda(&G_e);

	cT()->set_from_cpu(pameshParams->cT);
	pameshParams->cT.set_p_cu_obj_mpcuda(&cT);
	
	Q()->set_from_cpu(pameshParams->Q);
	pameshParams->Q.set_p_cu_obj_mpcuda(&Q);
}

Atom_MeshParamsCUDA::~Atom_MeshParamsCUDA()
{
	//fine to access data in MeshParams here : Mesh inherits from MeshParams, so in the destruction process Mesh gets destroyed first. 
	//Mesh destructor then calls for MeshCUDA implementation to be destroyed, then we get here since MeshCUDA inherits from MeshParamsCUDA. After this we return back to Mesh destructor to continue destruction down the list.

	//-----------SIMPLE CUBIC

	pameshParams->alpha.null_p_cu_obj_mpcuda();

	pameshParams->mu_s.null_p_cu_obj_mpcuda();

	pameshParams->J.null_p_cu_obj_mpcuda();

	pameshParams->K.null_p_cu_obj_mpcuda();
	pameshParams->mcanis_ea1.null_p_cu_obj_mpcuda();

	//-----------Others

	pameshParams->cHA.null_p_cu_obj_mpcuda();

	pameshParams->elecCond.null_p_cu_obj_mpcuda();

	pameshParams->thermCond.null_p_cu_obj_mpcuda();
	
	pameshParams->density.null_p_cu_obj_mpcuda();

	pameshParams->shc.null_p_cu_obj_mpcuda();
	pameshParams->shc_e.null_p_cu_obj_mpcuda();
	pameshParams->G_e.null_p_cu_obj_mpcuda();

	pameshParams->cT.null_p_cu_obj_mpcuda();
	
	pameshParams->Q.null_p_cu_obj_mpcuda();
}

#endif