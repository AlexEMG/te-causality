// Copyright 2012, Olav Stetter
// 
// This file is part of TE-Causality.
// 
// TE-Causality is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// TE-Causality is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with TE-Causality. If not, see <http://www.gnu.org/licenses/>.

#include "multipermutation.h"


// init
MultiPermutation::MultiPermutation(gsl_vector_int* ps) {
  if(ps->size < 1) {
    std::cout <<"error: init vector for MultiPermutation has zero length!"<<std::endl;
    exit(1);
  }
  permutation_elements = gsl_vector_int_alloc(ps->size);
  gsl_vector_int_memcpy(permutation_elements,ps);
  
  temp_access_vector = gsl_vector_int_alloc(required_length_of_access_vector());
  set_temp_vector_to_max_permutation_values();
  mem = new MultiDimArrayLong(temp_access_vector);
}

// destruct
MultiPermutation::~MultiPermutation() {
  delete mem;
  gsl_vector_int_free(permutation_elements);
  gsl_vector_int_free(temp_access_vector);
}

void MultiPermutation::set_temp_vector_to_max_permutation_values() {
  int element;
  int c = 0;
  for(int i=0; i<permutation_elements->size; i++) {
    element = gsl_vector_int_get(permutation_elements,i);
    for(int j=0; j<element; j++) { gsl_vector_int_set(temp_access_vector,c++,element); }
  }
}

int MultiPermutation::required_length_of_access_vector() {
  int total = 0;
  for(int i=0; i<permutation_elements->size; i++) {
    total += gsl_vector_int_get(permutation_elements,i);
  }
  return total;
}

long MultiPermutation::get(gsl_vector_int* access) {
  if(!test_validity_of_given_access_vector(access)) {
    std::cout <<"error: access vector for MultiPermutation#get is invalid!"<<std::endl;
    exit(1);
  }
  return mem->get(access);
}

void MultiPermutation::set(gsl_vector_int* access, long value) {
  if(!test_validity_of_given_access_vector(access)) {
    std::cout <<"error: access vector for MultiPermutation#set is invalid!"<<std::endl;
    exit(1);
  }
  return mem->set(access, value);
}

void MultiPermutation::clear() {
  // This is not implemented via MultiDimArrayLong#set_all because all the entries which
  // should be zero would also be changed, thus returnin a wrong result when calling the
  // function MultiPermutation#total.
  mem->clear();
}

long MultiPermutation::total() {
  return mem->total();
}

bool MultiPermutation::test_validity_of_given_access_vector(gsl_vector_int* access) {
  // test if length is correct
  if( access->size != required_length_of_access_vector() ) { return false; }
  
  // test if individual permutations are valid
  gsl_vector_int_set_zero(temp_access_vector);
  int offset = 0;
  int local_limit, access_element;
  for(int i=0; i<permutation_elements->size; i++) {
    local_limit = gsl_vector_int_get(permutation_elements, i);
    for(int j=0; j<local_limit; j++) {
      access_element = gsl_vector_int_get(access, offset+j);
      if( access_element < 0 ) { return false; }
      if( access_element >= local_limit ) { return false; }
      if( gsl_vector_int_get(temp_access_vector, access_element+offset) == 0 ) {
        gsl_vector_int_set(temp_access_vector, access_element+offset, 1);
      } else {
        std::cout <<"DEBUG: Two elements of this part of the multi-permutation are identical!"<<std::endl;
        return false;
      }
    }
    offset += local_limit;
  }
  return true;
}


// int MultiPermutation::factorial(int n) {
//   int result = n;
//   if(n > 170) {
//     std::cout <<"error: input to MultiPermutation::factorial() too large, overflow expected!"<<std::endl;
//     exit(1);
//   }
//   for(int i=1; i<n; i++) { result *= (n-i) }
//   return result;
// }

// Note: In this first iteration of the MultiPermutation object, we just use the MultiDimArrayLong
// object as before. We thus ignore the fact that this is memory inefficient because many entries
// must be zero because only permutations are allowed as access vectors.

// int MultiPermutation::encode_single_permutation_to_local_index(gsl_vector_int* perm, int first_index, int last_index) {
//   if(perm->size > last_index+1 || first_index > last_index) {
//     std::cout <<"error: arguments of MultiPermutation::encode_single_permutation_to_local_index() are invalid."<<std::endl;
//     exit(1);
//   }
//   const int max_number = last_index - first_index;
// }

// long MultiPermutation::maximum_value_for_given_index_based_on_access_vector(int dim) {
//   if( dim < 0 ) {
//     std::cout <<"error: argument of MultiPermutation::maximum_value_for_given_index_based_on_access_vector() is negative."<<std::endl;
//     exit(1);
//   }
//   
//   int remaining_dim = dim;
//   for(int i = 0; i < permutation_elements->size; i++) {
//     if( gsl_vector_int_get(permutation_elements,i) < remaining_dim) {
//       return i;
//     } else {
//       remaining_dim -= gsl_vector_int_get(permutation_elements,i);
//     }
//   }
//   
//   std::cout <<"error: argument of MultiPermutation::maximum_value_for_given_index_based_on_access_vector() is too large."<<std::endl;
//   exit(1);
//   return -1;
// }
