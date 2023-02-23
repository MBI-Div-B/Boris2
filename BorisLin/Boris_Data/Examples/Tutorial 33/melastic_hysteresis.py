"""
This script is part of BORIS

@author: Serban Lepadatu, 2023
"""

from NetSocks import NSClient, customize_plots
import matplotlib.pyplot as plt

#setup communication with server
ns = NSClient(); ns.configure(True); customize_plots()

########################################

#stress magnitude in Pa
stress_mag = 7e8

########################################

#1. No stress

FM = ns.Material('Fe', [320e-9, 320e-9, 5e-9], [2.5e-9, 2.5e-9, 5e-9])
FM.modules(['Zeeman', 'demag', 'exchange', 'anicubi', 'melastic'])
FM.pbc('x', 10)
FM.pbc('y', 10)

stress = 0.0
FM.setstress(stress, 90, 0)
output_file_nostress = 'melastic_stress_%.f.txt' % stress

ns.setode('LLGStatic', 'SDesc')

ns.setsavedata(output_file_nostress, ['Ha', FM], ['<M>', FM])

ns.cuda(1)
ns.Hpolar_seq([FM, [-100e3, 90, 1, 100e3, 90, 1, 100], 'mxh', 1e-5, 'step'])
ns.Hpolar_seq([FM, [100e3, 90, 1, -100e3, 90, 1, 100], 'mxh', 1e-5, 'step'])

plt.axes(xlabel = 'H (kA/m)', ylabel = 'M$_x$ (kA/m)')
hyster_nostress = ns.Get_Data_Columns(output_file_nostress, [0, 3])
plt.plot([H/1e3 for H in hyster_nostress[0]], [M/1e3 for M in hyster_nostress[1]], label = 'no stress')
plt.show()

########################################

#2. Compressive stress (hence reinforces easy axis along field)

stress = -stress_mag
FM.setstress(stress, 90, 0)
output_file_compressive = 'melastic_stress_%.f.txt' % stress
ns.savedatafile(output_file_compressive)

ns.Hpolar_seq([FM, [-100e3, 90, 1, 100e3, 90, 1, 100], 'mxh', 1e-5, 'step'])
ns.Hpolar_seq([FM, [100e3, 90, 1, -100e3, 90, 1, 100], 'mxh', 1e-5, 'step'])

plt.axes(xlabel = 'H (kA/m)', ylabel = 'M$_x$ (kA/m)')
hyster_compressive = ns.Get_Data_Columns(output_file_compressive, [0, 3])
plt.plot([H/1e3 for H in hyster_compressive[0]], [M/1e3 for M in hyster_compressive[1]], label = 'compressive stress: %.1f (MPa)' % (stress_mag/1e6))
plt.show()

########################################

#3. Extensive stress (hence weakens easy axis along field)

stress = stress_mag
FM.setstress(stress, 90, 0)
output_file_tensile = 'melastic_stress_%.f.txt' % stress
ns.savedatafile(output_file_tensile)

ns.Hpolar_seq([FM, [-100e3, 90, 1, 100e3, 90, 1, 100], 'mxh', 1e-5, 'step'])
ns.Hpolar_seq([FM, [100e3, 90, 1, -100e3, 90, 1, 100], 'mxh', 1e-5, 'step'])

plt.axes(xlabel = 'H (kA/m)', ylabel = 'M$_x$ (kA/m)')
hyster_extensive = ns.Get_Data_Columns(output_file_tensile, [0, 3])
plt.plot([H/1e3 for H in hyster_extensive[0]], [M/1e3 for M in hyster_extensive[1]], label = 'extensive stress: %.1f (MPa)' % (stress_mag/1e6))
plt.show()

########################################

plt.axes(xlabel = 'H (kA/m)', ylabel = 'M$_x$ (kA/m)')

hyster_nostress = ns.Get_Data_Columns(output_file_nostress, [0, 3])
plt.plot([H/1e3 for H in hyster_nostress[0]], [M/1e3 for M in hyster_nostress[1]], label = 'no stress')

hyster_compressive = ns.Get_Data_Columns(output_file_compressive, [0, 3])
plt.plot([H/1e3 for H in hyster_compressive[0]], [M/1e3 for M in hyster_compressive[1]], label = 'compressive stress: %.1f (MPa)' % (stress_mag/1e6))

hyster_extensive = ns.Get_Data_Columns(output_file_tensile, [0, 3])
plt.plot([H/1e3 for H in hyster_extensive[0]], [M/1e3 for M in hyster_extensive[1]], label = 'tensile stress: %.1f (MPa)' % (stress_mag/1e6))

plt.legend(loc = 'lower left')
plt.savefig('melastic.png', dpi = 600)
plt.show()
