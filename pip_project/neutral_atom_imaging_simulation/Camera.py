"""@package Camera
Module that implements different cameras"""

from abc import ABC, abstractmethod
import ctypes
import numpy as np
import typing

class Camera(ABC):
    """Abstract camera class"""
    @abstractmethod
    def get_image_creation_method(self):
        pass
    
    def set_zernike_coefficients(self, zernike_coefficients : typing.Union[np.ndarray, typing.Tuple[int,int,int,int,int,int,int,int,int,int,int,int,int,int,int]]):
        """Function for setting the zernike coefficients
        @param zernike_coefficients An array of exactly 15 values that represent the coefficients for
        -# piston
        -# y-tilt
        -# x-tilt
        -# defocus
        -# oblique astigmatism
        -# vertical astigmatism
        -# vertical coma
        -# horizontal coma
        -# vertical trefoil
        -# oblique trefoil
        -# primary spherical
        -# vertical secondary astigmatism
        -# oblique secondary astigmatism
        -# vertical quadrafoil
        -# oblique quadrafoil
        """
        self.zernike_coefficients = np.array(zernike_coefficients,np.float64)

    def set_library(self, library : ctypes.CDLL):
        """Function for setting the image generation library
        @param library The image generation C library
        @return None"""
        self.__library = library

    @property
    def library(self):
        """Function for getting the image generation library
        @return The image generation C library"""
        return self.__library

class EMCCDCamera(Camera):
    """Use this camera if the generated images should look like they are taken by an electron multiplying charge-coupled device (EMCCD) camera"""

    def __init__(self, resolution : typing.Tuple[int,int], dark_current_rate : float = None, cic_chance : float = None, quantum_efficiency : float = None, numerical_aperture : float = None, 
        physical_pixel_size : float = None, magnification : float = None, bias_clamp : float = None, preampgain : float = None, scic_chance : float = None, readout_stdev : float = None, 
        number_gain_reg : int = None, p0 : float = None, exposure_time : float = None, binning : int = 1):
        """Constructor
        -----------
        Initializes all camera specific parameters and relays them to the library
        @param resolution Number of pixels per dimension
        @param dark_current_rate Rate of dark current (photons/s)
        @param cic_chance Chance for any given pixel to generate a clock-induced charge (photons)
        @param quantum_efficiency Quantum efficiency of the camera for the corresponding wavelength\n
        [0.0,1.0]
        @param numerical_aperture Numerical apperture of the camera setup
        @param physical_pixel_size Physical pixel size of a single pixel (&#956m)
        @param magnification Magnification of the optical setup
        @param bias_clamp Bias clamp of the camera
        @param preampgain Preampgain of the camera
        @param scic_chance Chance for a serial clock-induced charge to occure in any given gain register\n
        [0.0,1.0]
        @param readout_stdev Standard deviation of the final readout
        @param number_gain_reg Number of gain registers
        @param p0 Chance for generating a secondary electron in any given gain register\n
        (1 + p0)^number_gain_reg = gain\n
        [0.0,1.0]
        @param exposure_time Exposure time (s)
        @param binning Binning factor for the final image"""
        self.dark_current_rate = dark_current_rate
        self.cic_chance = cic_chance
        self.quantum_efficiency = quantum_efficiency
        self.numerical_aperture = numerical_aperture
        self.physical_pixel_size = physical_pixel_size
        self.magnification = magnification
        self.bias_clamp = bias_clamp
        self.preampgain = preampgain
        self.scic_chance = scic_chance
        self.readout_stdev = readout_stdev
        self.number_gain_reg = number_gain_reg
        self.p0 = p0
        self.exposure_time = exposure_time
        self.binning = binning
        self.resolution = resolution
        self.zernike_coefficients = None

    def get_image_creation_method(self):
        """Function for acquiring the function handle of the library that is used to generate images using this camera
        @return The library function for generating images using this camera"""
        return self.library.createImageEMCCD
    
    def apply_settings(self):
        """Function for relaying any settings changes to the library
        @return None"""
        if self.dark_current_rate is not None:
            self.library.setDarkCurrentRate(ctypes.c_double(self.dark_current_rate))
        if self.cic_chance is not None:
            self.library.setCicChance(ctypes.c_double(self.cic_chance))
        if self.quantum_efficiency is not None:
            self.library.setQuantumEfficiency(ctypes.c_double(self.quantum_efficiency))
        if self.numerical_aperture is not None:
            self.library.setNumericalAperture(ctypes.c_double(self.numerical_aperture))
        if self.physical_pixel_size is not None:
            self.library.setPhysicalPixelSize(ctypes.c_double(self.physical_pixel_size))
        if self.magnification is not None:
            self.library.setMagnification(ctypes.c_double(self.magnification))
        if self.bias_clamp is not None:
            self.library.setBiasClamp(ctypes.c_double(self.bias_clamp))
        if self.preampgain is not None:
            self.library.setPreampgain(ctypes.c_double(self.preampgain))
        if self.scic_chance is not None:
            self.library.setSCICChance(ctypes.c_double(self.scic_chance))
        if self.readout_stdev is not None:
            self.library.setReadoutStdev(ctypes.c_double(self.readout_stdev))
        if self.number_gain_reg is not None:
            self.library.setNumberGainRegisters(ctypes.c_double(self.number_gain_reg))
        if self.p0 is not None:
            self.library.setP0(ctypes.c_double(self.p0))
        if self.exposure_time is not None:
            self.library.setExposureTime(ctypes.c_double(self.exposure_time))
        if self.binning is not None:
            self.library.setBinning(ctypes.c_int(self.binning))
        if (self.zernike_coefficients is not None) and len(self.zernike_coefficients) >= 15:
            self.library.setZernikeCoefficients(self.zernike_coefficients.ctypes.data_as(ctypes.POINTER(ctypes.c_double)))
        self.library.setResolution(ctypes.c_int(self.resolution[0]), ctypes.c_int(self.resolution[1]))

class CMOSCamera(Camera):
    """Use this camera if the generated images should look like they are taken by a CMOS camera"""

    def __init__(self, resolution : typing.Tuple[int,int], dark_current_sampling_alpha : float = None, dark_current_sampling_beta : float = None, 
        quantum_efficiency : float = None, numerical_aperture : float = None, physical_pixel_size : float = None, magnification : float = None, 
        bias_clamp : float = None, bias_stdev : float = None, row_noise_stdev : float = None, column_noise_scale : float = None, flicker_noise_scale : float = None, 
        preampgain : float = None, readout_stdev : float = None, exposure_time : float = None, binning : int = 1):
        """Constructor
        -----------
        Initializes all camera specific parameters and relays them to the library
        @param resolution Number of pixels per dimension
        @param dark_current_sampling_alpha Alpha value for sampling Gamma distribution for dark current
        @param dark_current_sampling_beta Beta value for sampling Gamma distribution for dark current
        @param quantum_efficiency Quantum efficiency of the camera for the corresponding wavelength\n
        [0.0,1.0]
        @param numerical_aperture Numerical apperture of the camera setup
        @param physical_pixel_size Physical pixel size of a single pixel (&#956m)
        @param magnification Magnification of the optical setup
        @param bias_clamp Bias clamp of the camera
        @param bias_stdev Standard deviation of the bias clamp
        @param row_noise_stdev Standard deviation of the row noise
        @param column_noise_scale Scale of the zero-mean Gumbel distribution for column noise
        @param flicker_noise_scale Scale of the zero-mean Gumbel distribution for flicker noise
        @param preampgain Preampgain of the camera
        @param readout_stdev Standard deviation of the final readout
        @param exposure_time Exposure time (s)
        @param binning Binning factor for the final image"""
        self.dark_current_sampling_alpha = dark_current_sampling_alpha
        self.dark_current_sampling_beta = dark_current_sampling_beta
        self.quantum_efficiency = quantum_efficiency
        self.numerical_aperture = numerical_aperture
        self.physical_pixel_size = physical_pixel_size
        self.magnification = magnification
        self.bias_clamp = bias_clamp
        self.bias_stdev = bias_stdev
        self.row_noise_stdev = row_noise_stdev
        self.column_noise_scale = column_noise_scale
        self.flicker_noise_scale = flicker_noise_scale
        self.preampgain = preampgain
        self.readout_stdev = readout_stdev
        self.exposure_time = exposure_time
        self.binning = binning
        self.resolution = resolution
        self.zernike_coefficients = None

    def get_image_creation_method(self):
        """Function for acquiring the function handle of the library that is used to generate images using this camera
        @return The library function for generating images using this camera"""
        return self.library.createImageCMOS
    
    def apply_settings(self):
        """Function for relaying any settings changes to the library
        @return None"""
        if self.dark_current_sampling_alpha is not None:
            self.library.setDarkCurrentSamplingAlpha(ctypes.c_double(self.dark_current_rate))
        if self.dark_current_sampling_beta is not None:
            self.library.setDarkCurrentSamplingBeta(ctypes.c_double(self.dark_current_rate))
        if self.quantum_efficiency is not None:
            self.library.setQuantumEfficiency(ctypes.c_double(self.quantum_efficiency))
        if self.numerical_aperture is not None:
            self.library.setNumericalAperture(ctypes.c_double(self.numerical_aperture))
        if self.physical_pixel_size is not None:
            self.library.setPhysicalPixelSize(ctypes.c_double(self.physical_pixel_size))
        if self.magnification is not None:
            self.library.setMagnification(ctypes.c_double(self.magnification))
        if self.bias_clamp is not None:
            self.library.setBiasClamp(ctypes.c_double(self.bias_clamp))
        if self.bias_stdev is not None:
            self.library.setBiasStdev(ctypes.c_double(self.bias_stdev))
        if self.row_noise_stdev is not None:
            self.library.setRowNoiseStdev(ctypes.c_double(self.row_noise_stdev))
        if self.column_noise_scale is not None:
            self.library.setColumnNoiseScale(ctypes.c_double(self.column_noise_scale))
        if self.flicker_noise_scale is not None:
            self.library.setFlickerNoiseScale(ctypes.c_double(self.flicker_noise_scale))
        if self.preampgain is not None:
            self.library.setPreampgain(ctypes.c_double(self.preampgain))
        if self.readout_stdev is not None:
            self.library.setReadoutStdev(ctypes.c_double(self.readout_stdev))
        if self.exposure_time is not None:
            self.library.setExposureTime(ctypes.c_double(self.exposure_time))
        if self.binning is not None:
            self.library.setBinning(ctypes.c_int(self.binning))
        if (self.zernike_coefficients is not None) and len(self.zernike_coefficients) >= 15:
            self.library.setZernikeCoefficients(self.zernike_coefficients.ctypes.data_as(ctypes.POINTER(ctypes.c_double)))
        self.library.setResolution(ctypes.c_int(self.resolution[0]), ctypes.c_int(self.resolution[1]))