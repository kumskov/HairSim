file(REMOVE_RECURSE
  "../bin/objectexploder.pdb"
  "../bin/objectexploder"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/objectexploder.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
