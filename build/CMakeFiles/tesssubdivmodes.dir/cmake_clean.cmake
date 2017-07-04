file(REMOVE_RECURSE
  "../bin/tesssubdivmodes.pdb"
  "../bin/tesssubdivmodes"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tesssubdivmodes.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
