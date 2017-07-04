file(REMOVE_RECURSE
  "../bin/dflandscape.pdb"
  "../bin/dflandscape"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/dflandscape.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
