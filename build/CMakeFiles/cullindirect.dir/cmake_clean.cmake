file(REMOVE_RECURSE
  "../bin/cullindirect.pdb"
  "../bin/cullindirect"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/cullindirect.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
