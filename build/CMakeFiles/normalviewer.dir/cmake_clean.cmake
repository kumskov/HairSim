file(REMOVE_RECURSE
  "../bin/normalviewer.pdb"
  "../bin/normalviewer"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/normalviewer.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
