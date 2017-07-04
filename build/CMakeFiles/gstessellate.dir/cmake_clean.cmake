file(REMOVE_RECURSE
  "../bin/gstessellate.pdb"
  "../bin/gstessellate"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/gstessellate.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
