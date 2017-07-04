file(REMOVE_RECURSE
  "../bin/phonglighting.pdb"
  "../bin/phonglighting"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/phonglighting.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
