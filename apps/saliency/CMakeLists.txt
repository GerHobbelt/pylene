FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

link_libraries(-lfreeimage)


QT4_WRAP_CPP(dispsaliency_HEADERS_MOC
  ${CMAKE_HOME_DIRECTORY}/mln/qt/imageviewer.hpp
  dispsaliency.hpp
  )


add_executable(dispsaliency dispsaliency.cpp
                      ${dispsaliency_HEADERS_MOC}
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/qtimage.cpp
                      ${CMAKE_HOME_DIRECTORY}/mln/qt/imageviewer.cpp
		      )



target_link_libraries(dispsaliency ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES})


add_executable(saliency saliency.cpp
  ${CMAKE_HOME_DIRECTORY}/apps/attributes/MSERArgparser.cpp
  ${CMAKE_HOME_DIRECTORY}/apps/attributes/meaningfullnessArgparser.cpp)

target_link_libraries(saliency ${Boost_PROGRAM_OPTIONS_LIBRARY})

#add_executable(tosview tosview.cpp)
