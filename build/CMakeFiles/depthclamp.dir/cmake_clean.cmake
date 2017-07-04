file(REMOVE_RECURSE
  "../bin/depthclamp.pdb"
  "../bin/depthclamp"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/depthclamp.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
