#!/bin/bash
cd venv/lib/python2.7/site-packages/
ln -s /usr/local/Cellar/opencv3/3.1.0_4/lib/python2.7/site-packages/cv2.so cv2.so
ln -s /usr/local/Cellar/pillow/3.4.2/lib/python2.7/site-packages/PIL PIL
cd ../../../../
