

add_library(qepicstimescan SHARED
  script.h
  script.cpp
  script.ui
  timescan.h
  timescan.ui
  timescan.cpp
)

target_link_libraries(qepicstimescan
  qtpv
  blitz
  poptmx
  Qt5::Widgets
  Qt5::PrintSupport
  ${QWT_LIBRARIES}
)

install(TARGETS qepicstimescan
    LIBRARY DESTINATION lib
)

install(FILES timescan.h
    DESTINATION include
)

install(FILES listOfKnownMotorPVs.txt
    DESTINATION share
)
