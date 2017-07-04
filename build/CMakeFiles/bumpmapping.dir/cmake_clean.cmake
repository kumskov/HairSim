file(REMOVE_RECURSE
  "../bin/bumpmapping.pdb"
  "../bin/bumpmapping"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/bumpmapping.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
