file(REMOVE_RECURSE
  "../bin/grass.pdb"
  "../bin/grass"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/grass.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
