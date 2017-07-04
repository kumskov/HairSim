file(REMOVE_RECURSE
  "../bin/hqfilter.pdb"
  "../bin/hqfilter"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/hqfilter.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
