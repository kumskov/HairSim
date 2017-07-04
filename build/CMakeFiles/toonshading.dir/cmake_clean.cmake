file(REMOVE_RECURSE
  "../bin/toonshading.pdb"
  "../bin/toonshading"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/toonshading.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
