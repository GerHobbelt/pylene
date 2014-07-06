FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

link_libraries(-lfreeimage)


QT4_WRAP_CPP(imview_HEADERS_MOC
  ${CMAKE_HOME_DIRECTORY}/mln/qt/imageviewer.hpp)


add_executable(imview imview.cpp
                      ${imview_HEADERS_MOC}
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/qtimage.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/imageviewer.cpp
		      )



target_link_libraries(imview ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES})


