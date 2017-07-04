file(REMOVE_RECURSE
  "../bin/noperspective.pdb"
  "../bin/noperspective"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/noperspective.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
