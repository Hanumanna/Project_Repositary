matrix:
  include:    
    - language: c  
      sudo: false # only for faster builds  
      compiler:
        - gcc
      script:  
        - gcc -o Parking Parking.c 
        - ./Parking

