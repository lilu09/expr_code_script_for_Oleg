#ifndef TUPLEX_H
#define TUPLEX_H


#include <bits/stdc++.h>





template<std::size_t>
struct int_{};

//tuple to ostream abstration
/*{{{*/

template <class ...Args>
void tuple_recursive_printer(std::ostream &os, std::tuple<Args...> const &t, int_<0>){

	os<<std::get<0>(t);

}

template <size_t I, class ...Args>
void tuple_recursive_printer(std::ostream &os, std::tuple<Args...> const &t, int_<I>){

	tuple_recursive_printer(os, t, int_<I-1>() );
	os<<","<<std::get<I>(t);
}


template <class ...Args>
std::ostream& operator<<(std::ostream &os, std::tuple<Args...> const &t){

	os<<"(";
	tuple_recursive_printer(os, t, int_<sizeof...(Args)-1>());
	os<<")"<<std::endl;

	return os;

}




struct tuple_ostream_iterator
: public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	tuple_ostream_iterator( std::ostream& os, std::string term )
		: os_(os)
		  , term_(std::move(term))
	{}

	template<typename... T>
		tuple_ostream_iterator& operator=( std::tuple<T...> const& elem )
		{
			os_ << elem << term_;
			return *this;
		}


	tuple_ostream_iterator& operator*() { return *this; }
	tuple_ostream_iterator& operator++() { return *this; }
	tuple_ostream_iterator& operator++( int ) { return *this; }

	private:
	std::ostream& os_;
	std::string term_;
};




/*}}}*/

//tuple to file abstration
/*{{{*/

template <class ...Args>
void tuple_recursive_writer(std::ofstream &os, std::tuple<Args...> const &t, int_<0>){

	os<<std::get<0>(t);

}

template <size_t I, class ...Args>
void tuple_recursive_writer(std::ofstream &os, std::tuple<Args...> const &t, int_<I>){

	tuple_recursive_writer(os, t, int_<I-1>() );
	os<<" "<<std::get<I>(t);
}


template <class ...Args>
std::ofstream& operator<<(std::ofstream &os, std::tuple<Args...> const &t){

	tuple_recursive_writer(os, t, int_<sizeof...(Args)-1>());
	os<<std::endl;

	return os;

}

struct tuple_ofstream_iterator
:public std::iterator<std::output_iterator_tag, void, void, void, void>
{

	explicit tuple_ofstream_iterator( std::ofstream& ofs, std::string term )
		: ofs_(ofs)
		  , term_(std::move(term))
	{}

	template<typename... T>
		tuple_ofstream_iterator& operator=( std::tuple<T...> const& elem )
		{
			ofs_ << elem << term_;
			return *this;
		}


	tuple_ofstream_iterator& operator*() { return *this; }
	tuple_ofstream_iterator& operator++() { return *this; }
	tuple_ofstream_iterator& operator++( int ) { return *this; }

	private:
	std::ofstream& ofs_;
	std::string term_;
};
/*}}}*/


//file to tuple abstration

/*

template <class... Args>
struct select_last;

template <typename T>
struct select_last<T>
{
     using type = T;
};

template <class T, class... Args>
struct select_last<T, Args...>
{
    using type = typename select_last<Args...>::type;
};

*/


template <class First, class ...Args>
void tuple_recursive_reader(std::istream &is, std::tuple<First, Args...> &t, int_<0>){

	is>>std::get<0>(t) ;

}

template <size_t I, class ...Args>
void tuple_recursive_reader(std::istream &is, std::tuple<Args...> &t, int_<I>){

	//using Last=typename select_last<Args...>::type;

	tuple_recursive_reader(is, t, int_<I-1>() );
	//is>>static_cast<Last>(std::get<I>(t) );
	is>>std::get<I>(t);
}


template <class ...Args>
std::ifstream& operator>>(std::ifstream &is, std::tuple<Args...> &t){

	std::string line;
	getline(is, line);
	std::istringstream iss(line);
	tuple_recursive_reader(iss, t, int_<sizeof...(Args)-1>());

	return is;

}

template <class ...Args>
void file_copy(std::ifstream &is, std::vector<std::tuple<Args...> > &v){
	std::tuple<Args...> t;
	while(is>>t){
		v.push_back(std::move(t));
	}
}

//hard to know what if.begin() returns, definitely not tuple_ifstream_iterator
//thus use file_copy directly

/*
struct tuple_ifstream_iterator
:public std::iterator<std::input_iterator_tag, void, void, void, void>
{

	explicit tuple_ifstream_iterator( std::ifstream& ifs, std::string term )
		: ifs_(ifs)
		  , term_(std::move(term))
	{}

	template<typename... T>
		tuple_ifstream_iterator& operator=( std::tuple<T...> const& elem )
		{
			ifs_ >> elem;
			return *this;
		}


	tuple_ifstream_iterator& operator*() { return *this; }
	tuple_ifstream_iterator& operator++() { return *this; }
	tuple_ifstream_iterator& operator++( int ) { return *this; }

	private:
	std::ifstream& ifs_;
	std::string term_;
};

*/


#endif
