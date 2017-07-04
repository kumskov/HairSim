file(REMOVE_RECURSE
  "../bin/alienrain.pdb"
  "../bin/alienrain"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/alienrain.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
