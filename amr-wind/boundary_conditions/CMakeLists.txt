target_sources(${amr_wind_lib_name}
   PRIVATE
      #C++
      BCInterface.cpp
      FixedGradientBC.cpp
      scalar_bcs.cpp
      MassInflowOutflowBC.cpp
   )

add_subdirectory(wall_models)
