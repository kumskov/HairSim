file(REMOVE_RECURSE
  "../bin/gsculling.pdb"
  "../bin/gsculling"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/gsculling.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
