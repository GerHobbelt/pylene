include(FindQwt)

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(Qwt REQUIRED)

INCLUDE(${QT_USE_FILE})
include_directories(${QWT_INCLUDE_DIR})


link_libraries(-lfreeimage)


QT4_WRAP_CPP(tosgui_HEADERS_MOC
  ${CMAKE_HOME_DIRECTORY}/mln/qt/mainwin.hpp
  qattribute.hpp
  dispatcher.hpp
  )

QT4_WRAP_CPP(tosgui_HEADERS_MOC
  ${CMAKE_HOME_DIRECTORY}/mln/qt/mainwin.hpp
  qattribute.hpp
  dispatcher.hpp
  )


add_executable(tosgui tosgui.cpp qattribute.cpp dispatcher.cpp
                      ${tosgui_HEADERS_MOC}
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/qtimage.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/mainwin.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/graphics_pixmap_item.cpp
		      )

add_executable(toscgui toscgui.cpp qattribute.cpp dispatcher.cpp
                      ${tosgui_HEADERS_MOC}
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/qtimage.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/mainwin.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/graphics_pixmap_item.cpp
		      )



target_link_libraries(tosgui
  ${QWT_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
  ${QT_LIBRARIES}
)

target_link_libraries(toscgui
  ${QWT_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
  ${QT_LIBRARIES}
)



