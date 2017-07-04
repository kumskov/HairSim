file(REMOVE_RECURSE
  "../bin/fontdemo.pdb"
  "../bin/fontdemo"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/fontdemo.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
