file(REMOVE_RECURSE
  "../bin/multiscissor.pdb"
  "../bin/multiscissor"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/multiscissor.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
