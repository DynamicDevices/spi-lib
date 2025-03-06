from scipy import constants
import numpy as np
import matplotlib.pyplot as plt

num_chirps = 16
num_samples = 128
chirp_repetition_time_s = 7e-05
frame_repetition_time_s = 5e-3
lower_frequency_Hz = 61020098000
upper_frequency_Hz = 61479902000 
sample_rate_Hz = 128 / 7e-05
bandwidth = upper_frequency_Hz - lower_frequency_Hz
sweep_slope_Hz_s = bandwidth / chirp_repetition_time_s
wavelength = constants.speed_of_light / lower_frequency_Hz

targets = [(5,0), (10,10)]

frame = np.zeros((num_chirps, num_samples))
for n in range(0, num_chirps):
    t = 0
    for i in range(0, num_samples):
        for target in targets:
            R = target[0]
            V = target[1]
            round_trip_delay_s = 2 * R / constants.speed_of_light
            phase = -2 * constants.pi * lower_frequency_Hz * 2 * V * n * chirp_repetition_time_s / constants.speed_of_light
            frequency = -2 * constants.pi * ((2 * V * (lower_frequency_Hz + (n * bandwidth)) / constants.speed_of_light) + (sweep_slope_Hz_s * round_trip_delay_s)) * t
            frame[n, i] += 0.5 * np.cos(phase + frequency)

        t = t + (1 / sample_rate_Hz)

frame_file = open("c:/Temp/frame.txt", "w")
print(frame.shape, file=frame_file)
for row in frame:
    np.savetxt(frame_file, row.astype(np.single))
frame_file.close()

range_bins = np.fft.fft(frame, axis=-1)
range_file = open("c:/Temp/range.txt", "w")
print(range_bins[..., :num_samples // 2].shape, file=range_file)
for row in range_bins[..., :num_samples // 2]:
    np.savetxt(range_file, row.astype(np.singlecomplex))
range_file.close()

range_bins = range_bins[..., :num_samples // 2]  # Real data is symmetric
rangeBinAxis = np.arange(0, num_samples // 2) * (constants.speed_of_light / ( 2 * bandwidth))

rdi = np.fft.fft(range_bins, axis=-2)  # Doppler FFT
rdi_file = open("c:/Temp/rdi.txt", "w")
print(rdi.shape, file=rdi_file)
for row in rdi.T:
    np.savetxt(rdi_file, row.astype(np.singlecomplex))
rdi_file.close()

rdi = np.fft.fftshift(rdi, axes=-2)  # Swap spectrum
dopplerBinSize = (1 / chirp_repetition_time_s) / num_chirps
velocityBinAxis = np.arange(-num_chirps // 2, num_chirps // 2) * dopplerBinSize * wavelength / 2

plt.plot(rangeBinAxis, np.abs(range_bins).T)
plt.xlabel('Range Bins')
plt.ylabel('Reflected Power')
plt.title('Interpreting a Single Chirp')
plt.show()

powers = np.abs(rdi)
# Plot with units
plt.imshow(powers.T, extent=[velocityBinAxis.min(), velocityBinAxis.max(), rangeBinAxis.max(), rangeBinAxis.min()])
plt.xlabel('Velocity (meters per second)')
plt.ylabel('Range (meters)')
plt.show()
