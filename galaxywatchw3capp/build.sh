#!/bin/bash

#tizen clean -- .
tizen build-native -r wearable-4.0-device.core -a arm -c llvm -C Debug -- .
tizen package -t tpk -s sanjeev -- ./Debug
