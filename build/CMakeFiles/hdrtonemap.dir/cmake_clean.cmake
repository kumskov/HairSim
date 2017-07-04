file(REMOVE_RECURSE
  "../bin/hdrtonemap.pdb"
  "../bin/hdrtonemap"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/hdrtonemap.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
