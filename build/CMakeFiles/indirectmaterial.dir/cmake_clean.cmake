file(REMOVE_RECURSE
  "../bin/indirectmaterial.pdb"
  "../bin/indirectmaterial"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/indirectmaterial.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
