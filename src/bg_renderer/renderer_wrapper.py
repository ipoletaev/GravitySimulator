# -*- coding: utf-8 -*-

from __future__ import absolute_import
from .ext_c import Renderer
import sys, traceback

class BackgroundGenerator:

	def __init__(self, img_h, img_w):

		print ('Create generator...')
		self.bg_generator = None
		self.__bg = None
		self.img_w, self.img_h = img_w, img_h

		try:
			self.bg_generator = Renderer(img_h=img_h, img_w=img_w)
			self.name = self.bg_generator.get_renderer_name()
			# TODO: thread exception while using PBO renderer: probably, GLUT isn't thread safe lib
			if self.name.rfind('Frame') >= 0:
				raise Exception('Frame buffer renderer isn\'t thread safe! Exit...')
		except Exception:
			self.show_exception_info()
			exit(-1)

	def init_background(self):

		try:
			self.__bg = self.bg_generator.get_initial_bg()
		except Exception:
			self.show_exception_info()
			exit(-1)

	def update_background(self, time):

		if self.data is not None:
			self.bg_generator.update(bg=self.__bg, time=time)

	def show_background(self):

		if self.data is not None:
			print self.__bg

	def save_background_in_img(self, path):

		import os
		path = os.path.join(path, 'bg_img.png')

		try:
			from skimage import io
			if self.data is not None:
				io.imsave(path, self.__bg)
		except ImportError:
			print 'Unable to save image without skimage lib! Install scikit-image'

	@property
	def data(self):
		return self.__bg.reshape((self.img_h, self.img_w))

	@staticmethod
	def show_exception_info():
		print 'Error:', sys.exc_info()[0]
		print traceback.print_exc(file=sys.stdout)