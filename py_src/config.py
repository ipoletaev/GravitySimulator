# -*- coding: utf-8 -*-

# -*- Misceleneous -*- #
LIVE_BACKGROUND = False # simulation of the flickering star sky
RUNNING, PAUSE = 1, 0  # some const variables

# -*- Buttons settings -*- #
BUTTON_WIDTH = 70
BUTTON_HEIGHT = 30
BUTTON_COLOR = (39, 135, 176)
START_BUTTON_CORNER = (BUTTON_HEIGHT, BUTTON_HEIGHT)
PAUSE_BUTTON_CORNER = (BUTTON_HEIGHT, BUTTON_HEIGHT * 2 + 10)
STOP_BUTTON_CORNER  = (BUTTON_HEIGHT, BUTTON_HEIGHT * 3 + 20)

# -*- Folders -*- #
IMAGES_OF_PLANETS_PATH = 'planets_images'

# -*- Physical parameters of simulation -*- #
GRAVITY_CONSTANT = 6.67
TIME_STEP = 0.2
PARTICLES_SCALE = 1/30.0
MAX_MASS = 20


# -*- Simulation mode settings -*- #
MODE = "gravitational_maneuver"
# possible options:
# "inner_solar_system"
# "planet_and_moon"
# "gravitational_maneuver"
# -*- -*- -*- -*- #
