file(REMOVE_RECURSE
  "../bin/raytracer.pdb"
  "../bin/raytracer"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/raytracer.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
