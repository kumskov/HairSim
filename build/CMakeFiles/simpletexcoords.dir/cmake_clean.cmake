file(REMOVE_RECURSE
  "../bin/simpletexcoords.pdb"
  "../bin/simpletexcoords"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/simpletexcoords.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
