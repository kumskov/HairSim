file(REMOVE_RECURSE
  "../bin/subroutines.pdb"
  "../bin/subroutines"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/subroutines.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
