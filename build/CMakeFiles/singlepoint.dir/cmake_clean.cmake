file(REMOVE_RECURSE
  "../bin/singlepoint.pdb"
  "../bin/singlepoint"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/singlepoint.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
