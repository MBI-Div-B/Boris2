"""
This script is part of BORIS

@author: Serban Lepadatu, 2020
"""

from NetSocks import NSClient
import matplotlib.pyplot as plt
import numpy as np

#setup communication with server
ns = NSClient(); ns.configure(True)

########################################

ns.loadsim('saf_bilayer')

ns.Run()

u = [np.cos(np.radians(1)), np.sin(np.radians(1)), 0]
ns.dp_load('saf_hysteresis', [0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7, 8])

ns.dp_dotprod(0, u[0], u[1], u[2], 10)
ns.dp_dotprod(3, u[0], u[1], u[2], 11)
ns.dp_dotprod(6, u[0], u[1], u[2], 12)
ns.dp_adddp(11, 12, 13)
ns.dp_div(13, 2.0)
ns.dp_save('saf_hysteresis_loop.txt', [10, 13])

loop = ns.Get_Data_Columns('saf_hysteresis_loop.txt', [0, 1])
plt.axes(xlabel = 'H (A/m)', ylabel = 'M (A/m)', title = 'SAF Hysteresis Loop')
plt.plot(loop[0], loop[1])
plt.show()


    
    

    


    



























