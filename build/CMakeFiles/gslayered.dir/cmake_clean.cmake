file(REMOVE_RECURSE
  "../bin/gslayered.pdb"
  "../bin/gslayered"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/gslayered.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
