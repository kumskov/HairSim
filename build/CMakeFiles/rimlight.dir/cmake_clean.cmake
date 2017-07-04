file(REMOVE_RECURSE
  "../bin/rimlight.pdb"
  "../bin/rimlight"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/rimlight.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
