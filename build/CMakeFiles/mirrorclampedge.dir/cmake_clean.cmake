file(REMOVE_RECURSE
  "../bin/mirrorclampedge.pdb"
  "../bin/mirrorclampedge"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/mirrorclampedge.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
