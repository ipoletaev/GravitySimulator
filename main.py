#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys, os
import traceback
import pygame
from pygame.locals import *
from pygame.time import get_ticks
from pygame.draw import lines
from threading import Condition
from py_src.gravity_processor import GravityProcessor
from py_src.background_processor import SimulatorSettings, BackGroundUpdater
from src.version_info import ENGINE_VERSION

PLANETS_IMAGES_PATH = os.path.join(os.path.dirname(__file__), './py_src/planets_images/')

# -*- -*- -*- -*- #
RUNNING, PAUSE = 1, 0
BUTTON_WIDTH = 70
BUTTON_HEIGHT = 30
BUTTON_COLOR = (39, 135, 176)
START_BUTTON_CORNER = (BUTTON_HEIGHT, BUTTON_HEIGHT)
PAUSE_BUTTON_CORNER = (BUTTON_HEIGHT, BUTTON_HEIGHT * 2 + 10)
STOP_BUTTON_CORNER =  (BUTTON_HEIGHT, BUTTON_HEIGHT * 3 + 20)
# -*- -*- -*- -*- #

LIVE_BACKGROUND = False # simulation of the flickering star sky

class GravitySimulator:

    def __init__(self, settings):

        self.__is_simulation_active = True
        self.settings = settings
        self.state = PAUSE
        self.size = (self.settings.window_width, self.settings.window_height)
        self.start_btn = self.pause_btn = self.stop_btn = False

    def start_simulation(self):

        print ('Start ...')
        pygame.init()
        self.screen = pygame.display.set_mode(self.size, 0 , 32)
        pygame.display.set_caption('Gravity Simulator v.' + ENGINE_VERSION)

        state = Condition()
        self.bg = BackGroundUpdater(img_w=self.settings.window_width,
                                    img_h=self.settings.window_height,
                                    transform_function=pygame.surfarray.make_surface,
                                    state=state)
        solver = GravityProcessor(self.load_planets(), self.settings.window_width, self.settings.window_height)

        time_to_exit = 0
        scene = self.bg.get_init_background()
        try:
            if LIVE_BACKGROUND:
                self.bg.start()
            while self.simulation_status or (time_to_exit < 2500):
                for e in pygame.event.get():
                    if e.type == QUIT:
                        self.simulation_status = False
                    elif e.type == KEYDOWN:
                        if e.key == K_ESCAPE:
                            self.simulation_status = False
                            time_to_exit = get_ticks()
                    elif e.type == MOUSEBUTTONDOWN:
                        if e.button == 1:
                            mouse_x, mouse_y = e.pos
                            self.get_pressed_button_type(mouse_x, mouse_y)

                # -*- background -*- #
                if self.state == RUNNING:
                    self.screen.blit(scene, (0, 0))
                    if LIVE_BACKGROUND:
                        #TODO: increase the smoothness of background updating
                        with state:
                            state.wait()
                            scene = self.bg.background

                    self.text(self.settings.window_width - 80, 10,
                             ('Time = %.1fs' % self.bg.get_time()), color=(0, 128, 255))

                    # particles positions update
                    solver.update()
                    self.draw_particles(solver.data_to_draw())
                # -*- -*- -*- -*- -*- #

                elif self.state == PAUSE and self.pause_btn and not self.start_btn:
                    self.text(self.settings.window_width / 2 - 72, self.settings.window_height / 2 - 72,
                              'Pause', size=72, font_name="comicsansms")

                elif self.stop_btn == True or (self.simulation_status == False):
                    self.screen.fill((0,0,0))
                    time_to_exit = (get_ticks() - time_to_exit)
                    self.text(self.settings.window_width / 2 - 100, self.settings.window_height / 2 - 72,
                              'EXIT...', size=72, font_name="comicsansms")

                elif not (self.start_btn and self.pause_btn and self.stop_btn):
                    self.text(self.settings.window_width / 2 - 250, self.settings.window_height / 2 - 72,
                              'Press \'START\'!', size=72, font_name="comicsansms")

                self.draw_buttons()
                pygame.display.update()

        except Exception:
            print 'Error:', sys.exc_info()[0]
            print traceback.print_exc(file=sys.stdout)

        except KeyboardInterrupt:
            print ('Interrupt simulation ...')

        finally:
            print ('Stop the executing ...')
            if not self.bg.deactivated.is_set():
                self.bg.stop()
            self.stop_simulation()

    def stop_simulation(self):

        pygame.time.delay(500)
        print ('Quit ...')
        pygame.quit()

    def draw_particles(self, particles):

        for particle in particles:
            #id = particle[1]
            image = particle[2]
            size = particle[3]
            image = pygame.transform.scale(image, (size, size))

            trajectory = [[int(x[0]), int(x[1])] for x in list(particle[0])]
            lines(self.screen, (255,255,255), False, trajectory, 2)

            point = trajectory[-1]
            self.screen.blit(image, (point[0] - size / 2, point[1] - size / 2))

    def load_planets(self):

        import glob
        planets = []
        for x in xrange(len(glob.glob(os.path.join(PLANETS_IMAGES_PATH, '*.png')))):
            planets.append(pygame.image.load(os.path.join(PLANETS_IMAGES_PATH, 'planet_{}.png'.format(x))))

        return planets

    def get_pressed_button_type(self, mouse_x, mouse_y):

        if (mouse_x >= START_BUTTON_CORNER[0]) and (mouse_x <= START_BUTTON_CORNER[0] + BUTTON_WIDTH) \
            and (mouse_y >= START_BUTTON_CORNER[1]) and (mouse_y <= START_BUTTON_CORNER[1] + BUTTON_HEIGHT):
            if self.start_btn == False and self.stop_btn != True:
                self.state = RUNNING
                self.bg.unpause_time()
                self.start_btn, self.pause_btn = True, False

        elif (mouse_x >= PAUSE_BUTTON_CORNER[0]) and (mouse_x <= PAUSE_BUTTON_CORNER[0] + BUTTON_WIDTH) \
            and (mouse_y >= PAUSE_BUTTON_CORNER[1]) and (mouse_y <= PAUSE_BUTTON_CORNER[1] + BUTTON_HEIGHT):
            if self.start_btn == True and self.stop_btn != True:
                self.state = PAUSE
                self.bg.pause_timer()
                self.start_btn, self.pause_btn = False, True

        elif (mouse_x >= STOP_BUTTON_CORNER[0]) and (mouse_x <= STOP_BUTTON_CORNER[0] + BUTTON_WIDTH) \
            and (mouse_y >= STOP_BUTTON_CORNER[1]) and (mouse_y <= STOP_BUTTON_CORNER[1] + BUTTON_HEIGHT):
            self.simulation_status = False
            self.state = PAUSE
            self.start_btn, self.pause_btn, self.stop_btn = False, False, True

    def draw_buttons(self):

        def get_text_color_of_button(status):

            if status == True:
                return Color("blue")
            else:
                return Color("white")

        def get_button_parameters(button):

            if button == "start":
                return (START_BUTTON_CORNER[0], START_BUTTON_CORNER[1], BUTTON_WIDTH, BUTTON_HEIGHT)
            elif button == "pause":
                return (PAUSE_BUTTON_CORNER[0], PAUSE_BUTTON_CORNER[1], BUTTON_WIDTH, BUTTON_HEIGHT)
            elif button == "stop":
                return (STOP_BUTTON_CORNER[0], STOP_BUTTON_CORNER[1], BUTTON_WIDTH, BUTTON_HEIGHT)

        pygame.draw.rect(self.screen, BUTTON_COLOR, get_button_parameters("start"))
        pygame.draw.rect(self.screen, BUTTON_COLOR, get_button_parameters("pause"))
        pygame.draw.rect(self.screen, BUTTON_COLOR, get_button_parameters("stop"))

        self.text(START_BUTTON_CORNER[0] + 15, START_BUTTON_CORNER[1] + 10, 'START', color=get_text_color_of_button(self.start_btn))
        self.text(PAUSE_BUTTON_CORNER[0] + 15, PAUSE_BUTTON_CORNER[1] + 10, 'PAUSE', color=get_text_color_of_button(self.pause_btn))
        self.text(STOP_BUTTON_CORNER[0] + 20, STOP_BUTTON_CORNER[1] + 10, 'STOP', color=get_text_color_of_button(self.stop_btn))

    def text(self, x, y, text, size=12, color=(255,255,255), font_name="arial"):

        return self.screen.blit(pygame.font.SysFont(font_name, size).render(text, True, color), (x, y))

    @property
    def simulation_status(self):
        return self.__is_simulation_active

    @simulation_status.setter
    def simulation_status(self, new_status):
        if isinstance(new_status, bool):
            self.__is_simulation_active = new_status
        else:
            raise TypeError('You should pass bool argument!')

def main():

    settings = SimulatorSettings()
    simulator = GravitySimulator(settings=settings)
    simulator.start_simulation()

if __name__ == '__main__':

    main()
