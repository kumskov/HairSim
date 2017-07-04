file(REMOVE_RECURSE
  "../bin/sampleshading.pdb"
  "../bin/sampleshading"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/sampleshading.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
