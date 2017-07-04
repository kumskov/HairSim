file(REMOVE_RECURSE
  "../bin/pmbfractal.pdb"
  "../bin/pmbfractal"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/pmbfractal.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
