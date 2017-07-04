file(REMOVE_RECURSE
  "../bin/ompparticles.pdb"
  "../bin/ompparticles"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ompparticles.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
