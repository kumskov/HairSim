file(REMOVE_RECURSE
  "../bin/cubemapenv.pdb"
  "../bin/cubemapenv"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/cubemapenv.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
