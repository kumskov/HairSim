file(REMOVE_RECURSE
  "../bin/linesmooth.pdb"
  "../bin/linesmooth"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/linesmooth.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
