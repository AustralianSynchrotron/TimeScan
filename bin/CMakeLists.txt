
add_executable(TimeScanMX
  TimeScanMX_main.cpp
  TimeScanMX_mainwindow.h
  TimeScanMX_mainwindow.cpp
  TimeScanMX_mainwindow.ui
  TimeScanMX.qrc
)

target_include_directories(TimeScanMX
  PRIVATE ../lib
)

target_link_libraries(TimeScanMX
  qepicstimescan
)

install(TARGETS TimeScanMX
    DESTINATION bin
)

install(FILES TimeScanIcon.png
    DESTINATION share/icons
)

install(FILES TimeScan.desktop
    DESTINATION share/applications
)
