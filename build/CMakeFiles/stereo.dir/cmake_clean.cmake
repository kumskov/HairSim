file(REMOVE_RECURSE
  "../bin/stereo.pdb"
  "../bin/stereo"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/stereo.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
