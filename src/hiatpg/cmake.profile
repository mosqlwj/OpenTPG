profile:
  - Debug:
      filter: 
        - Darwin
      cmake-options: '-DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -G "CodeBlocks - Unix Makefiles"'
      build-environments: 
        - MACOSX_DEPLOYMENT_TARGET: 10.9
      build-dir: 'cmake-build-debug'
      build-options: -j 8

  - Release:
      filter: 
        - Darwin
      cmake-options: '-DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -G "CodeBlocks - Unix Makefiles"'
      build-environments: 
        - MACOSX_DEPLOYMENT_TARGET: 10.9
      build-dir: 'cmake-build-debug'
      build-options: -j 8

