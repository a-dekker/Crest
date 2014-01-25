TARGET = harbour-crest

CONFIG += sailfishapp

SOURCES += src/Crest.cpp

QMAKE_CXXFLAGS += -std=c++0x

OTHER_FILES += \
    qml/pages/FirstPage.qml \
    rpm/harbour-crest.yaml \
    rpm/harbour-crest.spec \
    harbour-crest.desktop \
    qml/harbour-crest.qml \
    qml/pages/CoverPage.qml \
    qml/pages/ErrorPage.qml

HEADERS += \
    src/crest.h

