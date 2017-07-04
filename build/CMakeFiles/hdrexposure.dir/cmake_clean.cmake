file(REMOVE_RECURSE
  "../bin/hdrexposure.pdb"
  "../bin/hdrexposure"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/hdrexposure.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
