#include <bits/stdc++.h>

template <class MeasureType, class Func, class... Tunable_Args>
class tunable{
public:
	tunable(size_t _num_dimensions, size_t _num_variants, std::initializer_list<Func> const & dp):num_dimensions(_num_dimensions), num_variants(_num_variants), dispatch_table(dp){}
	//tunable(size_t _num_dimensions, size_t _num_variants):num_dimensions(_num_dimensions), num_variants(_num_variants){}
	//tuple format: seq_id, vid, val, size1, size2, size3
	//parameter: variant mask, repeat size, size1, size2, ...
	virtual std::vector<std::tuple<size_t, size_t, MeasureType, Tunable_Args...> > training_run(std::vector<bool> const& variant_mask, size_t repeat_size, Tunable_Args...) const =0;
	unsigned int const num_dimensions;
	unsigned int const num_variants;
	 //Func const dispatch_table[]; 
	 std::vector<Func> dispatch_table; 
	 //std::array<Func,NUM_VARIANTS> const dispatch_table; 
};
