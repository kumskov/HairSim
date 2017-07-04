file(REMOVE_RECURSE
  "../bin/basicfbo.pdb"
  "../bin/basicfbo"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/basicfbo.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
