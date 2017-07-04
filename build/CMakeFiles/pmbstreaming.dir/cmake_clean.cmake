file(REMOVE_RECURSE
  "../bin/pmbstreaming.pdb"
  "../bin/pmbstreaming"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/pmbstreaming.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
