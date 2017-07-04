file(REMOVE_RECURSE
  "../bin/springmass.pdb"
  "../bin/springmass"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/springmass.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
