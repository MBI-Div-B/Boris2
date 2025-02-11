#include "HeatBaseCUDA.h"

#if COMPILECUDA == 1

#ifdef MODULE_COMPILATION_HEAT

#include "BorisCUDALib.cuh"

//-------------------Setters

BError HeatBaseCUDA::SetQEquation(const std::vector< std::vector<EqComp::FSPEC> >& fspec)
{
	BError error(CLASS_STR(HeatBaseCUDA));

	if (!Q_equation.make_scalar(fspec)) return error(BERROR_OUTOFGPUMEMORY_CRIT);

	error = temp_cmbnd_funcs()->set_Q_equation(Q_equation);

	return error;
}

void HeatBaseCUDA::ClearQEquation(void)
{
	Q_equation.clear();
	temp_cmbnd_funcs()->set_Q_equation(Q_equation);
}

#endif

#endif