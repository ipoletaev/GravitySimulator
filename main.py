#!/usr/bin/env python
# -*- coding: utf-8 -*-
import py_src.config as config
import sys
import traceback
import pygame
from pygame.locals import *
from pygame.time import get_ticks
from pygame.draw import lines
from threading import Condition
from py_src.utils import SimulatorSettings, load_planets
from py_src.gravity_processor import GravityProcessor
from py_src.background_processor import BackGroundUpdater
from src.version_info import ENGINE_VERSION

class GravitySimulator:

    def __init__(self, settings):

        self.__is_simulation_active = True
        self.settings = settings
        self.state = config.PAUSE
        self.size = (self.settings.window_width, self.settings.window_height)
        self.start_btn = self.pause_btn = self.stop_btn = False
        self.time_to_exit = 0

    def start_simulation(self):

        def process_events():

            for e in pygame.event.get():
                if e.type == QUIT:
                    self.simulation_status = False
                elif e.type == KEYDOWN:
                    if e.key == K_ESCAPE:
                        self.simulation_status = False
                        self.time_to_exit = get_ticks()
                elif e.type == MOUSEBUTTONDOWN:
                    if e.button == 1:
                        mouse_x, mouse_y = e.pos
                        self.get_pressed_button_type(mouse_x, mouse_y)

        def process_animation():

            # -*- background -*- #
            if self.state == config.RUNNING:
                self.screen.blit(self.scene, (0, 0))

                if config.LIVE_BACKGROUND: #TODO: increase the smoothness of background updating
                    with state:
                        state.wait()
                        self.scene = self.bg.background

                self.text(self.settings.window_width - 80, 10,
                         ('Time = %.1fs' % self.bg.get_time()), color=(0, 128, 255))

                # particles positions update
                self.solver.update()
                self.draw_particles(self.solver.data_to_draw())
            # -*- -*- -*- -*- -*- #

            elif self.state == config.PAUSE and self.pause_btn and not self.start_btn:
                self.text(self.settings.window_width / 2 - 72, self.settings.window_height / 2 - 72,
                          'Pause', size=72, font_name="comicsansms")

            elif self.stop_btn == True or (self.simulation_status == False):
                self.screen.fill((0,0,0))
                self.time_to_exit = (get_ticks() - self.time_to_exit)
                self.text(self.settings.window_width / 2 - 100, self.settings.window_height / 2 - 72,
                          'EXIT...', size=72, font_name="comicsansms")

            elif not (self.start_btn and self.pause_btn and self.stop_btn):
                self.text(self.settings.window_width / 2 - 250, self.settings.window_height / 2 - 72,
                          'Press \'START\'!', size=72, font_name="comicsansms")

        print ('Start ...')
        pygame.init()
        self.screen = pygame.display.set_mode(self.size, 0, 32)
        pygame.display.set_caption('Gravity Simulator v.' + ENGINE_VERSION)

        # initialization of the renderer and trajectory calculator
        state = Condition()
        self.bg = BackGroundUpdater(img_w=self.settings.window_width,
                               img_h=self.settings.window_height,
                               transform_function=pygame.surfarray.make_surface,
                               state=state)
        self.solver = GravityProcessor(load_planets(), self.settings.window_width, self.settings.window_height)
        self.scene = self.bg.get_init_background()

        # main loop
        try:
            if config.LIVE_BACKGROUND:
                self.bg.start()
            while self.simulation_status or (self.time_to_exit < 2500):

                process_events()     # events processing

                process_animation() # process the animation

                self.draw_buttons()  # always draw buttons and update the screen
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

    def get_pressed_button_type(self, mouse_x, mouse_y):

        if (mouse_x >= config.START_BUTTON_CORNER[0]) and \
           (mouse_x <= config.START_BUTTON_CORNER[0] + config.BUTTON_WIDTH) and \
           (mouse_y >= config.START_BUTTON_CORNER[1]) and \
           (mouse_y <= config.START_BUTTON_CORNER[1] + config.BUTTON_HEIGHT):

            if self.start_btn == False and self.stop_btn != True:
                self.state = config.RUNNING
                self.bg.unpause_time()
                self.start_btn, self.pause_btn = True, False

        elif (mouse_x >= config.PAUSE_BUTTON_CORNER[0]) and \
             (mouse_x <= config.PAUSE_BUTTON_CORNER[0] + config.BUTTON_WIDTH) and \
             (mouse_y >= config.PAUSE_BUTTON_CORNER[1]) and \
             (mouse_y <= config.PAUSE_BUTTON_CORNER[1] + config.BUTTON_HEIGHT):

            if self.start_btn == True and self.stop_btn != True:
                self.state = config.PAUSE
                self.bg.pause_timer()
                self.start_btn, self.pause_btn = False, True

        elif (mouse_x >= config.STOP_BUTTON_CORNER[0]) and \
             (mouse_x <= config.STOP_BUTTON_CORNER[0] + config.BUTTON_WIDTH) and \
             (mouse_y >= config.STOP_BUTTON_CORNER[1]) and \
             (mouse_y <= config.STOP_BUTTON_CORNER[1] + config.BUTTON_HEIGHT):

            self.simulation_status = False
            self.state = config.PAUSE
            self.start_btn, self.pause_btn, self.stop_btn = False, False, True

    def draw_buttons(self):

        #TODO: add the restart button

        def get_text_color_of_button(status):

            if status == True:
                return Color("blue")
            else:
                return Color("white")

        def get_button_parameters(button):

            if button == "start":
                return (config.START_BUTTON_CORNER[0], config.START_BUTTON_CORNER[1],
                        config.BUTTON_WIDTH, config.BUTTON_HEIGHT)
            elif button == "pause":
                return (config.PAUSE_BUTTON_CORNER[0], config.PAUSE_BUTTON_CORNER[1],
                        config.BUTTON_WIDTH, config.BUTTON_HEIGHT)
            elif button == "stop":
                return (config.STOP_BUTTON_CORNER[0], config.STOP_BUTTON_CORNER[1],
                        config.BUTTON_WIDTH, config.BUTTON_HEIGHT)

        pygame.draw.rect(self.screen, config.BUTTON_COLOR, get_button_parameters("start"))
        pygame.draw.rect(self.screen, config.BUTTON_COLOR, get_button_parameters("pause"))
        pygame.draw.rect(self.screen, config.BUTTON_COLOR, get_button_parameters("stop"))

        self.text(config.START_BUTTON_CORNER[0] + 15, config.START_BUTTON_CORNER[1] + 10, 'START',
                  color=get_text_color_of_button(self.start_btn))
        self.text(config.PAUSE_BUTTON_CORNER[0] + 15, config.PAUSE_BUTTON_CORNER[1] + 10, 'PAUSE',
                  color=get_text_color_of_button(self.pause_btn))
        self.text(config.STOP_BUTTON_CORNER[0] + 20, config.STOP_BUTTON_CORNER[1] + 10, 'STOP',
                  color=get_text_color_of_button(self.stop_btn))

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
