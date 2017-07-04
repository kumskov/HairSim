file(REMOVE_RECURSE
  "../bin/movingtri.pdb"
  "../bin/movingtri"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/movingtri.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
