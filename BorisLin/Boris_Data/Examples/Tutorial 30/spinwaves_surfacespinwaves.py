"""
This script is part of BORIS

@author: Serban Lepadatu, 2023
"""

from NetSocks import NSClient, customize_plots
import matplotlib.pyplot as plt
import numpy as np
import scipy as sp
import scipy.fftpack
import scipy.signal

#NOTE: we need embedded scripting here so we can extract profiles during the simulation
#Unembedded scripting also possible but requires using the command buffering method (see manual) which is less elegant
ns = NSClient(embedded = True); ns.configure(True); customize_plots()

##############################################################
#NOTES:

#Backward volume : Bias field along length (excitation along width, analysed m component along width)
#Forward volume : Bias field along thickness (excitation along width, analysed m component along width)
#Surface spin wave : Bias field along width (excitation along length, analysed m component along length)

#In all cases the wave-vector is along length, which means the extracted magnetisation profile is along length.
#Also the only constraint on excitation and analysed m component direction is they have to be perpendicular to the bias field.

##############################################################
#Setup

#fmr cutoff frequency (Hz)
fc = 500e9
#time step for saving magnetisation (s) : determined by Nyquist criterion from cutoff frequency
time_step = (0.5 / fc)

#k vector cutoff (rad / m)
kc = 2*np.pi*0.1255e9
#the mesh step used to obtain M values along the k vector direction; again this must be set from Nyquist criterion
#thus mesh_step = 0.5 / (kc / 2PI), which gives 4 nm
mesh_step = 4e-9

#mesh dimensions specified in IEEETM 49, 524 (2013)
meshdim = [1e-6, 50e-9, 1e-9]
#cellsize set to a small enough value: 1 nm along length is sufficient
#if this value is too high (2 nm) significant discrepancy between simulations and analytical lines results
cellsize = [1e-9, 2e-9, 1e-9]

#sinc pulse time centre (s) and total time to simulate (s)
#increase t0 if you want better resolution
t0 = 200e-12
total_time = 2 * t0

#bias field and excitation strength (A/m)
H0 = 804e3
He = 400e3

output_file = 'spinwaves_ssw.txt'

#define useful constants
A = 1.3e-11
Ms = 800e3
gamma = 2.212761569e5
damping = 0.01
mu0 = 4*np.pi*1e-7
lambda_ex = 2*A / (mu0*Ms**2)
wM = gamma * Ms

##############################################################
#setup mesh dimensions, pbc (10 images along x only), set magnetisation along +x direction

Py = ns.Ferromagnet(meshdim, cellsize)
Py.modules(['Zeeman', 'demag', 'exchange'])
Py.pbc('x', 10)
Py.setangle(90, 90)

Py.param.damping = damping
Py.param.Ms = Ms
Py.param.A = A
Py.param.grel = gamma / 2.212761569e5

#set fixed time-step RK4 method with a small enough time step (50 fs is sufficient - small value due to 1 nm cellsize)
ns.setode('LLG', 'RK4')
dT = 50e-15
ns.setdt(dT)

########################################
#Run

def custom_data_save(time_last):
    time = ns.showdata('time')
    
    #trigger saves every time_step
    if time_step - (time - time_last) < dT or time < dT:
        time_last = time
        ns.dp_getexactprofile([0, meshdim[1]/2, meshdim[2]/2], [meshdim[0], meshdim[1]/2, meshdim[2]/2], mesh_step, 0)
        ns.dp_div(1, Ms)
        ns.dp_saveappendasrow(output_file, 1)
        
    return 1, time_last
    
#make sure output file is wiped clean
ns.dp_newfile(output_file)

ns.cuda(1)
#setup sinc pulse using a formula
#first define equation constants
#Bias field (A/m)
ns.equationconstants('H0', H0)
#Excitation field (A/m)
ns.equationconstants('He', He)
#k cutoff (rad/m)
ns.equationconstants('kc', kc)
#f cutoff (Hz) -> hence time step of 1ps due to Nyquist criterion
ns.equationconstants('fc', fc)
#time center (s)
ns.equationconstants('t0', t0)
#now run
ns.Hequation([Py, 'He * sinc(kc*(x-Lx/2))*sinc(kc*(y-Ly/2))*sinc(2*PI*fc*(t-t0)), H0, 0', 'time', total_time], custom_data_save, 0.0)

########################################

#Analyse

#get 2D list as position along horizontal, time along vertical
pos_time = ns.Get_Data_Columns(output_file)

#2D fft (use log function to improve image contrast)
fourier_data = np.fft.fftshift(np.abs(sp.fftpack.fft2(pos_time)))

#get value ranges
freq_len = len(fourier_data)
k_len = len(fourier_data[0])
freq = sp.fftpack.fftfreq(freq_len, time_step)
kvector = sp.fftpack.fftfreq(k_len, mesh_step)

#maximum k and f values
k_max = 2*np.pi*kvector[int(0.5 * len(kvector))]
f_min = np.abs(freq[0])
f_max = np.abs(freq[int(0.5 * len(freq))])
f_points = int(0.5 * freq_len)

#extract result from fourier data in a plottable form
result = [fourier_data[i] for i in range(int(0.5 * freq_len),freq_len)]

#frequency range so we can plot the k = 0 profile
frange = np.arange(f_min, f_max, (f_max - f_min) / f_points)

#extract peaks above a sloping threshold line
peaks_threshold = 0.5
peaks_slope = -0.08/100e9
thresh_line = [peaks_threshold + peaks_slope * f for f in frange]

#extract k = 0 line and get peak f values using threshold line
k0line_plot = [result[i][int(0.5*k_len)] for i in range(int(0.5 * freq_len))]
k0line = [result[i][int(0.5*k_len)] - peaks_slope * frange[i] for i in range(int(0.5 * freq_len))]
peaks,_ = sp.signal.find_peaks(k0line, height = peaks_threshold)

#plot and show peak f values
plt.plot(frange, k0line_plot)
plt.plot(frange, thresh_line, '--')
plt.show()

#print peak f (GHz) values and convert to w
fpeaks = [frange[peak]/1e9 for peak in peaks]
wpeaks = [frange[peak]*2*np.pi for peak in peaks]
print("f peak values (GHz) : ")
print(fpeaks)

#plot spin wave dispersion
plt.axes(xlabel = 'k (rad/m)', ylabel = 'f (Hz)', title = 'Surface Spin Waves')
plt.imshow(result, origin='lower', interpolation='bilinear', 
           extent = [-k_max, k_max, f_min, f_max], 
           aspect ="auto")

#plot analytical lines
krange = np.arange(-k_max, k_max, 1e7)

for wn in wpeaks:
    
    f = [(wn + wM * lambda_ex * k**2)/(2*np.pi) for k in krange]
    plt.plot(krange, f, 'r:')

#restrict plotting limits
plt.ylim(f_min, f_max)
plt.xlim(-k_max, k_max)

plt.savefig('surfacespinwaves.png', dpi = 600)
plt.show()