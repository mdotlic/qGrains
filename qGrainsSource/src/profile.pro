TEMPLATE = app
TARGET = qGrains_1.34

CONFIG += qt \
c++11 

RESOURCES = qGrains.qrc

QT += widgets printsupport

INCLUDEPATH += /home/mdotlic/share/NN/thirdparty/nn/

LIBS += -L"/media/Storage/Institut/qGrains/qGrains/qGrainsSource/thirdparty/nn/" -lnn

SOURCES += main.cpp \
           qGrains.cpp \
           centralWindow.cpp \
           mainMenu.cpp \
           inputTab/inputTab.cpp\
           inputTab/pictureWidget/pictureWidget.cpp\
           inputTab/pictureWidget/picture.cpp\
           inputTab/pictureWidget/predefList.cpp\
           inputTab/leftWidget.cpp\
           plotTab/plotTab.cpp\
           plotTab/plotLeftWidget.cpp\
           plotTab/plotPicture.cpp\
           depthTab/depthTab.cpp \
           depthTab/depthDTab/depthDTab.cpp\
           depthTab/depthDTab/chooserWidget.cpp\
           depthTab/depthDTab/depthDPicture.cpp\
           depthTab/depthDTab/drillChooser.cpp\
           depthTab/depthDTab/dChooser/dChooser.cpp\
           depthTab/percTab/percTab.cpp\
           depthTab/percTab/percChooser.cpp\
           depthTab/percTab/drillPercChooser.cpp\
           depthTab/percTab/percPicture.cpp\
           depthTab/percTab/sizeChooser/sizeChooser.cpp\
           condTab/condTab.cpp\
           condTab/condLeftWidget.cpp\
           condTab/condTable.cpp\
           profileTab/profileTab.cpp\
           profileTab/profileLeftWidget.cpp\
           profileTab/profilePicture.cpp\
           profileTab/profileMap.cpp\
           profileTab/profileSmallTabs/profileSmallTabs.cpp\
           profileTab/profileSmallTabs/profileChooser.cpp\
           profileTab/profileSmallTabs/surfaceChooser.cpp\
           planTab/planTab.cpp\
           planTab/planLeftWidget.cpp\
           planTab/planChooser.cpp\
           planTab/planPicture.cpp\
           planTab/isolinesWidget.cpp\
           model/handlers/handler.cpp\
           model/handlers/inputHandler.cpp\
           model/handlers/plotHandler.cpp\
           model/handlers/viewHandler.cpp\ 
           model/handlers/dHandler.cpp\
           model/handlers/sizeHandler.cpp\
           model/handlers/condHandler.cpp\
           model/handlers/surfHandler.cpp\
           model/handlers/profileHandler.cpp\
           model/modelNodeBase.cpp\
           model/pointSet.cpp\
           model/intPointSet.cpp\
           model/tables/tables.cpp\
           model/tables/myTableView.cpp\
           model/tables/surfTable.cpp\
           model/tables/profileTable.cpp\
           ../thirdparty/qcustomplot/qcustomplot.cpp\
           widgets/comboBoxDelegate.cpp\
           widgets/isoList.cpp\
           dialogs/message.cpp\
           dialogs/about.cpp\

HEADERS += qGrains.h \
           centralWindow.h\
           mainMenu.h \
           inputTab/inputTab.h\
           inputTab/pictureWidget/pictureWidget.h\
           inputTab/pictureWidget/picture.h\
           inputTab/pictureWidget/predefList.h\
           inputTab/leftWidget.h\
           plotTab/plotTab.h\
           plotTab/plotLeftWidget.h\
           plotTab/plotPicture.h\
           depthTab/depthTab.h\
           depthTab/depthDTab/depthDTab.h\
           depthTab/depthDTab/chooserWidget.h\
           depthTab/depthDTab/depthDPicture.h\
           depthTab/depthDTab/drillChooser.h\
           depthTab/depthDTab/dChooser/dChooser.h\
           depthTab/percTab/percTab.h\
           depthTab/percTab/percChooser.h\
           depthTab/percTab/drillPercChooser.h\
           depthTab/percTab/percPicture.h\
           depthTab/percTab/sizeChooser/sizeChooser.h\
           condTab/condTab.h\
           condTab/condLeftWidget.h\
           condTab/condTable.h\
           profileTab/profileTab.h\
           profileTab/profileLeftWidget.h\
           profileTab/profilePicture.h\
           profileTab/profileMap.h\
           profileTab/profileSmallTabs/profileSmallTabs.h\
           profileTab/profileSmallTabs/profileChooser.h\
           profileTab/profileSmallTabs/surfaceChooser.h\
           planTab/planTab.h\
           planTab/planLeftWidget.h\
           planTab/planChooser.h\
           planTab/planPicture.h\
           planTab/isolinesWidget.h\
           model/handlers/handler.h\
           model/handlers/inputHandler.h\
           model/handlers/plotHandler.h\
           model/handlers/viewHandler.h\
           model/handlers/dHandler.h\
           model/handlers/sizeHandler.h\
           model/handlers/condHandler.h\
           model/handlers/surfHandler.h\
           model/handlers/profileHandler.h\
           model/modelNodeBase.h\
           model/modelEnums.h\
           model/pointSet.h\
           model/intPointSet.h\
           model/tables/depthHeaderNames.h\
           model/tables/depthHeaderLineColorNames.h\
           model/tables/coorHeaderNames.h\
           model/tables/tables.h\
           model/tables/myTableView.h\
           model/tables/surfTable.h\
           model/tables/profileTable.h\
           calculation/calculation.h\
           calculation/extendedNN.hh\
           ../thirdparty/qcustomplot/qcustomplot.h\
           widgets/comboBoxDelegate.h\
           widgets/isoList.h\
           dialogs/message.h\
           dialogs/about.h\
           dxf/dxf.h\
