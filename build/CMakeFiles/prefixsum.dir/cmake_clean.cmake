file(REMOVE_RECURSE
  "../bin/prefixsum.pdb"
  "../bin/prefixsum"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/prefixsum.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
