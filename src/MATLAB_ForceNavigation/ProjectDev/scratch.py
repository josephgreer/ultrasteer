from __future__ import absolute_import
from __future__ import print_function
import numpy as np
np.random.seed(1337) # for reproducibility

from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import containers
from keras.layers.core import Dense, AutoEncoder
from keras.optimizers import RMSprop
from keras.utils import np_utils

batch_size = 64
nb_classes = 10
nb_epoch = 1

# the data, shuffled and split between train and test sets
(X_train, _), (X_test, _) = mnist.load_data()
X_train = X_train.reshape(-1, 784)
X_test = X_test.reshape(-1, 784)
X_train = X_train.astype("float32") / 255.0
X_test = X_test.astype("float32") / 255.0
print(X_train.shape[0], 'train samples')
print(X_test.shape[0], 'test samples')

#creating the autoencoder
ae = Sequential()
encoder = containers.Sequential([Dense(784, 700), Dense(700, 600)])
decoder = containers.Sequential([Dense(600, 700), Dense(700, 784)])
ae.add(AutoEncoder(encoder=encoder, decoder=decoder,
                   output_reconstruction=True, tie_weights=True))

ae.compile(loss='mean_squared_error', optimizer=RMSprop())
ae.fit(X_train, X_train, batch_size=batch_size, nb_epoch=nb_epoch,
       show_accuracy=False, verbose=1, validation_data=[X_test, X_test])