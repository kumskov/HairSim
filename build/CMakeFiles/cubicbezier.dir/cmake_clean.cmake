file(REMOVE_RECURSE
  "../bin/cubicbezier.pdb"
  "../bin/cubicbezier"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/cubicbezier.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
