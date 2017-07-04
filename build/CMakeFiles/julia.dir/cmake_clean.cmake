file(REMOVE_RECURSE
  "../bin/julia.pdb"
  "../bin/julia"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/julia.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
