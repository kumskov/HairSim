file(REMOVE_RECURSE
  "../bin/clipdistance.pdb"
  "../bin/clipdistance"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/clipdistance.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
