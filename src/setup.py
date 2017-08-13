# -*- coding: utf-8 -*-

from sys import platform
import os
import numpy as np
from distutils.core import setup, Extension
from version_info import ENGINE_VERSION

## -------------- ##

def transform_list(item, arr):
    new_arr = [arr[int(i/2)] if i % 2 != 0 else item for i in xrange(2 * len(arr))]
    return new_arr

CURRENT_DIR=os.getcwd()

if platform == "linux" or platform == "linux2":
    frameworks = transform_list('-l', ['glut', 'GLU', 'GL',  'm'])
elif platform == "darwin":
    frameworks = transform_list('-framework', ['GLUT', 'OpenGL', 'Cocoa'])
else:
    raise Exception('Unknown operating system type!')

include_dirs_for_cpp_code=[os.path.join(CURRENT_DIR, 'bg_renderer_cpp/', folder) for folder in ('renderer', 'open_gl_ext')]
include_dirs_for_cpp_code.extend(['./bg_renderer/', np.get_include()])

## -------------- ##

extra_compile_args = [
    '-std=c++11',
]

sources = []
for dirpath, dirnames, filenames in os.walk(CURRENT_DIR):
    for filename in [f for f in filenames if f.endswith(".cpp")]:
	if (filename.rfind('main.cpp') < 0):
            sources.append(os.path.join(dirpath, filename))

module = Extension(
    'bg_renderer.ext_c',
    include_dirs=include_dirs_for_cpp_code,
    sources=sources,
    extra_compile_args=extra_compile_args,
    extra_link_args=frameworks
)

packages = [
    'bg_renderer'
]

## -------------- ##

setup(
    name='bg_renderer',
    version=ENGINE_VERSION,
    packages=packages,
    description='Generator of the background images of star sky written in C++',
    ext_modules=[module],
)
