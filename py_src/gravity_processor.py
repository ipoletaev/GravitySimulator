# -*- coding: utf-8 -*-
import config
import math
import numpy as np
from collections import deque
import copy
from utils import create_satellite_image

#TODO: rewrite this part of code in C/C++

class Particle:

    """ Particle with some physical properties and state variables: position and velocity """

    def __init__(self, id, mass, image, size=None, position=None, velocity=None):

        self.__mass = mass
        if not size:
            self.__size = 20
        else:
            self.__size = int(size)
        self.__image = image

        if position is None:
            self.__position = [0, 0]
        else:
            if isinstance(position, list) and len(position) == 2:
                self.__position = position
            else:
                raise TypeError('Wrong position arg: list with two numbers is required!')

        if velocity is None:
            self.__velocity = [0.0, 0.0]
        else:
            if isinstance(velocity, list) and len(velocity) == 2:
                self.__velocity = velocity
            else:
                raise TypeError('Wrong velocity arg: list with two numbers is required!')

        self.id = id
        self.trajectory = deque(maxlen=300) # choose higher number for mor smooth trajectory line
        self.trajectory.append(copy.copy(position))

    @property
    def size(self):
        return self.__size

    @property
    def mass(self):
        return self.__mass

    @property
    def image(self):
        return self.__image

    def set_size(self, new_size):
        self.__size = new_size

    def set_mass(self, new_mass):
        self.__mass = new_mass

    def set_image(self, new_image):
        self.__image = new_image

    #########

    @property
    def position(self):
        return self.__position

    #########

    @property
    def velocity(self):
        return self.__velocity

    #########

    def update(self, acceleration, centroid_speed, centred=True):

        self.trajectory.append(copy.copy(self.position))

        # the simplest method for the integration of motion equations
        # https://en.wikipedia.org/wiki/Verlet_integration

        self.__position[0] += (self.__velocity[0] * config.TIME_STEP + acceleration[0] * (config.TIME_STEP ** 2)) / 2.0
        self.__position[1] += (self.__velocity[1] * config.TIME_STEP + acceleration[1] * (config.TIME_STEP ** 2)) / 2.0

        self.__velocity[0] += acceleration[0] * config.TIME_STEP
        self.__velocity[1] += acceleration[1] * config.TIME_STEP

        self.__position[0] += (self.__velocity[0] * config.TIME_STEP) / 2.0
        self.__position[1] += (self.__velocity[1] * config.TIME_STEP) / 2.0

        if centred: # draw picture centered by the system's mass center
            self.__position[0] -= centroid_speed[0] * config.TIME_STEP
            self.__position[1] -= centroid_speed[1] * config.TIME_STEP

    def get_info(self):

        return [self.trajectory, self.id, self.image, self.size]

class GravityProcessor:

    """ Simulator of gravity interaction between the particles with specific characteristics """

    def __init__(self, planets_images, img_w, img_h, particles_number=1):

        self.img_w, self.img_h = img_w, img_h
        self.particles_images = planets_images
        assert len(planets_images) >= particles_number

        if config.MODE == "inner_solar_system":
            self.particles, self.centroid_speed = self.inner_solar_system()
        elif config.MODE == "planet_and_moon":
            self.particles, self.centroid_speed = self.planet_and_moon()
        elif config.MODE == "slingshot":
            self.particles, self.centroid_speed = self.slingshot()
        elif config.MODE == "double_slingshot":
            self.particles, self.centroid_speed = self.double_slingshot()
        else:
            raise Exception('Wrong mode type! Exit..')
        self.particles_number = len(self.particles)

    def update(self):

        for i, part_1 in enumerate(self.particles):
            F_x, F_y = 0., 0.
            for j, part_2 in enumerate(self.particles):
                if i != j:
                    F = self.calc_force(i, j, part_1.mass, part_2.mass, part_1.position, part_2.position)
                    F_x += F[0]
                    F_y += F[1]
            a_x, a_y = F_x / part_1.mass, F_y / part_1.mass
            part_1.update([a_x, a_y], self.centroid_speed)

    def data_to_draw(self):

        return [p.get_info() for p in self.particles]

    def get_size(self, mass):

        return config.PARTICLES_SCALE * self.img_w * math.pow(mass / config.MAX_MASS, 1/3.)

    def calc_force(self, id_1, id_2, m_1, m_2, position_1, position_2):

        # F = - G * m_1 * m_2 * r / |r^3|

        r_x = position_2[0] - position_1[0]
        r_y = position_2[1] - position_1[1]
        r = math.sqrt(r_x ** 2 + r_y ** 2)

        F_x = config.GRAVITY_CONSTANT * m_1 * m_2 * r_x / (r ** 3)
        F_y = config.GRAVITY_CONSTANT * m_1 * m_2 * r_y / (r ** 3)

        return [F_x, F_y] # force's components on the plane

    def calc_centered_velocity(self, particles):

        vel = np.array([0.0,0.0])
        sum_mass = 0.

        for particle in particles:
            vel += np.array(particle.velocity) * particle.mass
            sum_mass += particle.mass

        return (vel / sum_mass).tolist()

    ### DIFFERENT MODES METHODS ###

    def inner_solar_system(self):

        # inner solar system with almost correct proportions

        sun_mass = 332000
        sun = Particle(0, sun_mass, self.particles_images[0], self.get_size(sun_mass / 1000),
                         position=[self.img_w / 5., self.img_h /2],
                         velocity=[0, 0])

        masses = [5.5, 81.5, 100, 10.7]
        sizes = [self.get_size(mass) for mass in masses]
        distances = np.array([0.38, 0.72, 1.0, 1.52]) * (self.img_w / 2.) + (self.img_w / 5.)
        velocities = [math.sqrt(config.GRAVITY_CONSTANT * sun_mass / dist) for dist in distances]

        planets = []
        for i, (init_v, mass) in enumerate(zip(velocities, masses)):
            planets.append(
                Particle(i + 1, mass, self.particles_images[i + 1], sizes[i],
                         position=[distances[i], self.img_h /2],
                         velocity=[0, init_v])
            )

        planets = [sun] + planets
        return planets, self.calc_centered_velocity(planets)

    def planet_and_moon(self):

        # example of the planet and its moon motion

        sun_mass = 30000
        sun_position = self.img_w / 2
        sun = Particle(1, sun_mass, self.particles_images[0], self.get_size(sun_mass / 100),
                         position=[sun_position, self.img_h /2],
                         velocity=[0, 0])

        dist = self.img_w / 4.
        dist_to_earth = sun_position + dist
        earth = Particle(2, 200, self.particles_images[3], self.get_size(100),
                         position=[dist_to_earth, self.img_h / 2],
                         velocity=[0, math.sqrt(config.GRAVITY_CONSTANT * sun_mass / dist)])

        moon = Particle(3, 0.05, self.particles_images[1], self.get_size(100 / 81.),
                         position=[dist_to_earth - dist / 20., self.img_h / 2],
                         velocity=[0, earth.velocity[1] / 1.5])

        planets = [sun, earth, moon]
        return planets,  self.calc_centered_velocity(planets)

    def slingshot(self):

        # Acceleration of the satellite during a flight near Jupiter

        sun_mass = 30000
        sun_position = self.img_w / 2
        sun = Particle(1, sun_mass, self.particles_images[0], self.get_size(sun_mass / 100),
                       position=[sun_position, self.img_h /2], velocity=[0, 0])

        dist = self.img_w / 3.
        dist_to_jupiter = sun_position + dist
        jupiter = Particle(2, 500, self.particles_images[5], self.get_size(100),
                           position=[dist_to_jupiter, self.img_h / 2],
                           velocity=[0, math.sqrt(config.GRAVITY_CONSTANT * sun_mass / dist)])

        satellite_img = create_satellite_image()
        satellite = Particle(3, 0.01, satellite_img, self.get_size(100 / 81.),
                             position=[21 * self.img_w / 100, self.img_h / 2],
                             velocity=[0, -21.6])


        planets = [sun, jupiter, satellite]
        return planets, self.calc_centered_velocity(planets)

    def double_slingshot(self):

        # Acceleration of the satellite during a flight near Earth and Venus

        sun_mass = 30000
        sun_position = self.img_w / 2
        sun = Particle(1, sun_mass, self.particles_images[0], self.get_size(sun_mass / 100),
                       position=[sun_position, self.img_h /2], velocity=[0, 0])

        dist = self.img_w * 0.35
        dist_to_venus = sun_position
        venus = Particle(2, 80, self.particles_images[2], self.get_size(80),
                         position=[dist_to_venus, 3 * self.img_h / 4],
                         velocity=[-math.sqrt(config.GRAVITY_CONSTANT * sun_mass / (0.72 * dist)), 0])

        dist_to_earth = sun_position + dist
        earth = Particle(2, 110, self.particles_images[3], self.get_size(100),
                         position=[dist_to_earth, self.img_h / 2],
                         velocity=[0, math.sqrt(config.GRAVITY_CONSTANT * sun_mass / dist)])

        satellite_img = create_satellite_image()
        satellite = Particle(3, 0.005, satellite_img, self.get_size(1.3),
                             position=[(sun_position - self.img_w / 3.) * 1.15, self.img_h / 2],
                             velocity=[14, -22])

        planets = [sun, venus, earth, satellite]
        return planets, self.calc_centered_velocity(planets)