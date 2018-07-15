#
#  QCustomPlot Plot Examples
#

QT += core gui
QT += widgets printsupport


# use relative path assuming cosmos source is 2 levels above
COSMOS = $$PWD/../../../
COSMOS_SOURCE = $$PWD/../../
# cosmos/source/tools/CosmosPlot/
#COSMOS_SOURCE = $$COSMOS/source/

TARGET = CosmosPlot
TEMPLATE = app
CONFIG += console
CONFIG += c++11

MODULES += elapsedtime
MODULES += timeutils
MODULES += socketlib
MODULES += agentlib
MODULES += jsonlib
MODULES += mathlib
MODULES += timelib
MODULES += datalib
MODULES += convertlib
MODULES += convertdef
MODULES += stringlib
MODULES += jpleph
MODULES += ephemlib
MODULES += geomag
MODULES += sliplib
MODULES += zlib
MODULES += print_utils
MODULES += objlib

#message( $$COSMOS_SOURCE)
include( $$COSMOS_SOURCE/core/cosmos-core.pri ) #removed /source/ to work on 527 linux machine

SOURCES += main.cpp\
           mainwindow.cpp \
           plot.cpp \
    editwindow.cpp

HEADERS  += mainwindow.h \
            plot.h \
    editwindow.h

# QCustomPlot
SOURCES += qcustomplot/qcustomplot.cpp
HEADERS += qcustomplot/qcustomplot.h


FORMS    += mainwindow.ui


json.files += plot-adcs.json
json.files += plot-adcs-minimal.json
json.files += plot-motion-tracker.json
#json.files += plot-arduino.json
#json.files += plot-cpu.json

#json.path = $$OUT_PWD
json.path = $$COSMOS/tools/CosmosPlot/
INSTALLS += json

# install path
target.path = $$COSMOS/tools/CosmosPlot
INSTALLS += target
