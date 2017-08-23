# -*- coding: utf-8 -*-
from threading import Thread, Event
from bg_renderer import BackgroundGenerator
import numpy as np
import time

class BackGroundUpdater(Thread):

    """
    This class allows to update the background in a separate thread from the main.
    """

    def __init__(self, img_w, img_h, transform_function, state):

        super(BackGroundUpdater, self).__init__()

        self.transformer = transform_function
        self.generator = BackgroundGenerator(img_w=img_w, img_h=img_h)
        self.generator.init_background()
        self.__data = self.get_init_background()

        self.max_time = self.generator.bg_generator.get_max_time()
        self.__start_time = time.time()
        self.pause_timer()

        self.__state = state
        self.daemon = True
        self.pause = True
        self.deactivated = Event()

    def run(self):
        while not self.deactivated.is_set():
            if not self.pause:

                data = self.prepare_data()
                with self.__state:
                    self.__data = data
                    self.__state.notify()

    def stop(self):
        with self.__state:
            self.deactivated.set()
            self.__state.notify()

    def get_time(self):

        current_time = time.time() - self.__start_time
        if current_time > self.max_time:
            current_time = 0
            self.__start_time = 0
        return current_time

    def pause_timer(self):

        self.pause_start_time = time.time()
        self.pause = True

    def unpause_time(self):

        self.pause_start_time = time.time() - self.pause_start_time
        self.__start_time += self.pause_start_time
        self.pause = False

    @property
    def background(self):
        return self.__data

    def get_init_background(self):

        self.generator.init_background()
        return self.transformer(np.repeat(self.generator.data.T[:, :, np.newaxis], 3, axis=2))

    def prepare_data(self):

        self.generator.update_background(self.get_time())
        data = self.generator.data                                   # linear array to matrix
        data = np.repeat(data.T[:, :, np.newaxis], 3, axis=2)        # grayscale to RGB
        return self.transformer(data)