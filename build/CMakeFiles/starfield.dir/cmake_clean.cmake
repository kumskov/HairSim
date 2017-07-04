file(REMOVE_RECURSE
  "../bin/starfield.pdb"
  "../bin/starfield"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/starfield.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
