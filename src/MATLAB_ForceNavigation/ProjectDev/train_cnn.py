from __future__ import absolute_import
from __future__ import print_function
import numpy as np
np.random.seed(1337) # for reproducibility
import glob2
import sys
import scipy

from keras.models import Sequential
from keras.layers import containers
from keras.layers.core import Dense, AutoEncoder
from keras.optimizers import RMSprop

tileNames = glob2.glob('Data/**/Tiles/*.png')

tiles = None

for tileName in tileNames:
    tile = scipy.ndimage.imread(tileName)
    tile = np.expand_dims(tile,axis=0)
    if tiles is None:
        tiles = tile
    else:
        tiles = np.concatenate((tiles,tile),axis=0)

np.save(tiles, 'tiles.dat')