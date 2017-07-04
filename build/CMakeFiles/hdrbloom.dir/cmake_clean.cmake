file(REMOVE_RECURSE
  "../bin/hdrbloom.pdb"
  "../bin/hdrbloom"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/hdrbloom.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
