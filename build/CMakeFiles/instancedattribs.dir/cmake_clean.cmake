file(REMOVE_RECURSE
  "../bin/instancedattribs.pdb"
  "../bin/instancedattribs"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/instancedattribs.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
