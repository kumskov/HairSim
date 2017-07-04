file(REMOVE_RECURSE
  "../bin/blinnphong.pdb"
  "../bin/blinnphong"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/blinnphong.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
