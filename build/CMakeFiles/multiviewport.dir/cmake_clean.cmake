file(REMOVE_RECURSE
  "../bin/multiviewport.pdb"
  "../bin/multiviewport"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/multiviewport.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
