file(REMOVE_RECURSE
  "../bin/shapedpoints.pdb"
  "../bin/shapedpoints"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/shapedpoints.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
