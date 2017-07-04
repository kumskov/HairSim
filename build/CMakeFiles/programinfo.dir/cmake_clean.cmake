file(REMOVE_RECURSE
  "../bin/programinfo.pdb"
  "../bin/programinfo"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/programinfo.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
