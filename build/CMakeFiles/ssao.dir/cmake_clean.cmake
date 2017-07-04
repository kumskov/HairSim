file(REMOVE_RECURSE
  "../bin/ssao.pdb"
  "../bin/ssao"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ssao.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
