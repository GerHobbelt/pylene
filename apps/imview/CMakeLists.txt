FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

link_libraries(-lfreeimage)

add_executable(imview imview.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/qtimage.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/mainwin.cpp)

target_link_libraries(imview ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES})


