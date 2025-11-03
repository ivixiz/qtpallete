QT += widgets x11extras
LIBS += -lX11 -lXcursor -lXcomposite -lXdamage 
TARGET  = colorpicker
DESTDIR = ../

HEADERS += \
    core/qtpalette.h \ 
    core/qtcontrol.h \ 
    core/recentcolors.h \ 
    core/control_by_wheel.h \ 
    core/screeneyedropper.h \
    
SOURCES += \
    main.cpp \
    core/qtpalette.cpp \
    core/qtcontrol.cpp \
    core/recentcolors.cpp \
    core/control_by_wheel.cpp \
    core/screeneyedropper.cpp \
