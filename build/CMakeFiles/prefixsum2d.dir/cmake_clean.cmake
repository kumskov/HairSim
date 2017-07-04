file(REMOVE_RECURSE
  "../bin/prefixsum2d.pdb"
  "../bin/prefixsum2d"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/prefixsum2d.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
