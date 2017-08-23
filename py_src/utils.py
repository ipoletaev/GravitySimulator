# -*- coding: utf-8 -*-
import config
from collections import namedtuple
import os
import glob
import pygame

IMAGES_OF_PLANETS_PATH = os.path.join(os.path.dirname(__file__), config.IMAGES_OF_PLANETS_PATH)

def load_planets():

    planets = []
    for x in xrange(len(glob.glob(os.path.join(IMAGES_OF_PLANETS_PATH, '*.png')))):
        planets.append(pygame.image.load(os.path.join(IMAGES_OF_PLANETS_PATH, 'planet_{}.png'.format(x))))

    return planets

class SimulatorSettings:

    __default_settings = namedtuple('Settings', ['window_width', 'window_height'])
    __default_settings.__new__.__defaults__ = (900, 650)

    def __init__(self):

        default_settings = SimulatorSettings.__default_settings()
        self.window_width = default_settings.window_width
        self.window_height = default_settings.window_height

    def set_window_size(self, width, height):

        self.window_width, self.window_height = width, height

def create_satellite_image():

    # create satellite image surface
    satellite_img = pygame.Surface((30, 30), pygame.SRCALPHA, 32)
    satellite_img.fill((0, 0, 0, 0))
    pygame.draw.circle(satellite_img, pygame.Color("blue"), (15, 15), 10)

    return satellite_img