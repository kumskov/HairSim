file(REMOVE_RECURSE
  "../bin/msaanative.pdb"
  "../bin/msaanative"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/msaanative.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
