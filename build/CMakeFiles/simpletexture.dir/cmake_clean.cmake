file(REMOVE_RECURSE
  "../bin/simpletexture.pdb"
  "../bin/simpletexture"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/simpletexture.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
