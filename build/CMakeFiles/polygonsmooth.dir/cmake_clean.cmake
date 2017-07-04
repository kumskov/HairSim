file(REMOVE_RECURSE
  "../bin/polygonsmooth.pdb"
  "../bin/polygonsmooth"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/polygonsmooth.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
