"""@package Experiment
Module that implements different experiment setups"""

from abc import ABC, abstractmethod
import ctypes
import math
import numpy as np
from random import random

class Experiment(ABC):
    """Abstract experiment class"""

    @abstractmethod
    def get_atom_sites(self):
        pass

    @abstractmethod
    def uses_camera_coords(self):
        pass

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
    
class TweezerArray(Experiment):
    """Use this if a tweezer array is to be simulated"""

    def __init__(self, stray_light_rate = None, imaging_wavelength = None, scattering_rate = None, survival_probability = None, fill_rate = None):
        """Constructor
        @param stray_light_rate Rate of stray light (photons/s)
        @param imaging_wavelength The imaging wavelength (um)
        @param scattering_rate The average of photons emitted by an atom per second (photons/s)
        @param survival_probability The chance for an atom to survive being imaged\n
        [0.0,1.0]
        @param fill_rate The chance for an atom site to be filled"""
        self.stray_light_rate = stray_light_rate
        self.imaging_wavelength = imaging_wavelength
        self.scattering_rate = scattering_rate
        self.survival_probability = survival_probability
        self.fill_rate = fill_rate
        self.atom_sites = []
        self.__uses_camera_coords = True
    
    def apply_settings(self):
        """Function for relaying any settings changes to the library
        @return None"""
        if self.stray_light_rate:
            self.library.setStrayLightRate(ctypes.c_double(self.stray_light_rate))
        if self.imaging_wavelength:
            self.library.setWavelength(ctypes.c_double(self.imaging_wavelength))
        if self.scattering_rate:
            self.library.setScatteringRate(ctypes.c_double(self.scattering_rate))
        if self.survival_probability:
            self.library.setSurvivalProbability(ctypes.c_double(self.survival_probability))
        if self.fill_rate:
            self.library.setFillingRatio(ctypes.c_double(self.fill_rate))

    def set_atom_sites_camera_space(self, atom_sites):
        """Function for setting the list of atom sites in normalized camera coordinates.
        @param atom_sites List of coordinate tuples where atom may reside, each coordinate value should be within [0.0,1.0]
        @return None"""
        self.__uses_camera_coords = True
        self.atom_sites = atom_sites

    def configure_atom_sites_camera_space(self, spacing: tuple, count: tuple, offset: tuple, angle : float):
        """Function for configuring the layout of atom sites in normalized camera coordinates. On average, there will be column_count * row_count * fill_rate atoms.
        @param spacing The distance between two columns of atoms in normalized camera coordinates (x,y)
        @param count The number of columns (x,y)
        @param offset Distance from the image edge to the first atom site in normalized camera coordinates (x,y)
        @param angle The whole grid is rotated around the first atom site by this angle. Specify in radians
        @return None"""
        self.__uses_camera_coords = True
        self.atom_sites = []
        rotated_column_spacing = np.array((spacing[0] * math.cos(angle), spacing[0] * math.sin(angle)))
        rotated_row_spacing = np.array((-spacing[1] * math.sin(angle), spacing[1] * math.cos(angle)))
        base = np.array(offset)
        for row in range(count[1]):
            for col in range(count[0]):
                self.atom_sites.append(tuple(base + col * rotated_column_spacing + row * rotated_row_spacing))

    def set_atom_sites_physical_space(self, atom_sites):
        """Function for setting the list of atom sites in physical object space.
        @param atom_sites List of coordinate tuples where atom may reside, each coordinate value should be within [0.0,1.0]
        @return None"""
        self.__uses_camera_coords = False
        self.atom_sites = atom_sites

    def configure_atom_sites_physical_space(self, spacing: tuple, count: tuple, offset: tuple, angle : float):
        """Function for configuring the layout of atom sites in physical object space. On average, there will be column_count * row_count * fill_rate atoms.
        @param spacing The distance between two columns of atoms in &#956m (x,y)
        @param count The number of columns (x,y)
        @param offset Distance from the image edge to the first atom site in &#956m (x,y)
        @param angle The whole grid is rotated around the first atom site by this angle. Specify in radians
        @return None"""
        self.__uses_camera_coords = False
        self.atom_sites = []
        rotated_column_spacing = np.array((spacing[0] * math.cos(angle), spacing[0] * math.sin(angle)))
        rotated_row_spacing = np.array((-spacing[1] * math.sin(angle), spacing[1] * math.cos(angle)))
        base = np.array(offset)
        for row in range(count[1]):
            for col in range(count[0]):
                self.atom_sites.append(tuple(base + col * rotated_column_spacing + row * rotated_row_spacing))

    def get_atom_sites(self):
        """Function for acquiring the list of atom sites
        @return The list of sites"""
        return self.atom_sites
    
    def uses_camera_coords(self):
        return self.__uses_camera_coords