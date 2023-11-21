"""@package ImageGenerator
Module for generating images of neutral atoms in a grid"""

import ctypes
from .Camera import Camera, EMCCDCamera
from .Experiment import Experiment, TweezerArray
import numpy as np
import matplotlib.pyplot as plt
from os import path
import platform

class ImageGenerator:
    """Main class for generating images"""
    __create_image_library = None
    __camera = None

    def __init__(self):
        """Constructor
        Loads C library for later use"""
        if platform.system() == 'Windows':
            self.__create_image_library = ctypes.windll.LoadLibrary(path.dirname(__file__) + '/lib/createSampleImage.dll')
        else:
            self.__create_image_library = ctypes.cdll.LoadLibrary(path.dirname(__file__) + '/lib/libcreateSampleImage.so')
        self.__create_image_library.readConfig.argtypes = [ctypes.c_char_p]

    def get_library(self):
        """Returns the loaded C library
        @return The image generation C library"""
        return self.__create_image_library

    def set_camera(self, camera : Camera):
        """Function for setting the camera
        @param camera The camera to be used for imaging
        @return None"""
        self.__camera = camera
        self.__camera.set_library(self.__create_image_library)
        self.__camera.apply_settings()
    
    def set_experiment(self, experiment : Experiment):
        """Function for setting the experiment to be imaged
        @param experiment The experiment to be imaged
        @return None"""
        self.__experiment = experiment
        self.__experiment.set_library(self.__create_image_library)
        self.__experiment.apply_settings()
    
    def get_psf(self, resolution: int):
        psf = np.zeros((resolution * resolution,))
        self.get_library().getConvolutedLightSource(psf.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), resolution)
        return psf.reshape((resolution,resolution))

    def create_image(self, approximation_steps = 1):
        """Function to be called for generating an image
        @param approximation_steps The number of subdivisions for each pixel for the optical simulation
        @return Numpy array of generated image
        @return Numpy array of ground truths per atom site"""
        resolution = (self.__camera.resolution[0] // self.__camera.binning, self.__camera.resolution[1] // self.__camera.binning)
        image = np.zeros((resolution[0] * resolution[1],))
        atom_locations = self.__experiment.get_atom_sites()
        atom_count = len(atom_locations)
        c_atom_list = (ctypes.c_double * 2 * atom_count)()
        truth = np.zeros((atom_count), np.float64)
        for j in range(atom_count):
            c_atom_list[j][0] = atom_locations[j][0]
            c_atom_list[j][1] = atom_locations[j][1]
        self.__camera.get_image_creation_method()(image.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), c_atom_list,\
            ctypes.c_int(self.__experiment.uses_camera_coords()), truth.ctypes.data_as(ctypes.POINTER(ctypes.c_double)), atom_count, approximation_steps)
        return image.reshape((resolution[1],resolution[0])), truth
    
    def read_config_file(self, path: str):
        self.__create_image_library.readConfig(path.encode('utf-8'))
