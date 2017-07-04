file(REMOVE_RECURSE
  "../bin/ktxview.pdb"
  "../bin/ktxview"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ktxview.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
