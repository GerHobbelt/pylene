FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

link_libraries(-lfreeimage)


QT4_WRAP_CPP(supervised-cli_MOC
  ${CMAKE_HOME_DIRECTORY}/mln/qt/imageviewer.hpp
  brush.hpp)

QT4_GENERATE_MOC(${CMAKE_HOME_DIRECTORY}/mln/qt/imageviewer.hxx
                 ${CMAKE_CURRENT_BINARY_DIR}/imageviewer.moc.cpp)

add_executable(supervised-cli  supervised-cli.cpp brush.cpp
                      ${supervised-cli_MOC}
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/qtimage.cpp
                      ${CMAKE_CURRENT_BINARY_DIR}/imageviewer.moc.cpp)



target_link_libraries(supervised-cli ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES})


