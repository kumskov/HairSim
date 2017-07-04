file(REMOVE_RECURSE
  "../bin/indexedcube.pdb"
  "../bin/indexedcube"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/indexedcube.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
