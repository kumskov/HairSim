file(REMOVE_RECURSE
  "../bin/tunnel.pdb"
  "../bin/tunnel"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tunnel.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
