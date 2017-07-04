file(REMOVE_RECURSE
  "../bin/dispmap.pdb"
  "../bin/dispmap"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/dispmap.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
