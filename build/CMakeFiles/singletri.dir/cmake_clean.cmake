file(REMOVE_RECURSE
  "../bin/singletri.pdb"
  "../bin/singletri"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/singletri.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
