file(REMOVE_RECURSE
  "../bin/shadowmapping.pdb"
  "../bin/shadowmapping"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/shadowmapping.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
