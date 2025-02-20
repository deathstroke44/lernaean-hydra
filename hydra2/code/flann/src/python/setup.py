#!/usr/bin/env python2

from distutils.core import setup
from os.path import exists, abspath, dirname, join
import os
import sys


def find_path():
    lib_paths = [ os.path.abspath('/home/karimae/projects/lernaean-hydra/hydra2/code/flann/lib'), abspath(join(dirname(dirname(sys.argv[0])), '../../../lib')) ]
    possible_libs = ['libflann.so', 'flann.dll', 'libflann.dll', 'libflann.dylib']

    for path in lib_paths:
        for lib in possible_libs:
            if exists(join(path,lib)):
                return path

setup(name='flann',
      version='1.9.1',
      description='Fast Library for Approximate Nearest Neighbors',
      author='Marius Muja',
      author_email='mariusm@cs.ubc.ca',
      license='BSD',
      url='http://www.cs.ubc.ca/~mariusm/flann/',
      packages=['pyflann', 'pyflann.lib'],
      package_dir={'pyflann.lib': find_path() },
      package_data={'pyflann.lib': ['libflann.so', 'flann.dll', 'libflann.dll', 'libflann.dylib']}, 
)
