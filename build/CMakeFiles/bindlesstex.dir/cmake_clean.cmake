file(REMOVE_RECURSE
  "../bin/bindlesstex.pdb"
  "../bin/bindlesstex"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/bindlesstex.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
