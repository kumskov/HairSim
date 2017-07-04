file(REMOVE_RECURSE
  "../bin/dof.pdb"
  "../bin/dof"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/dof.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
