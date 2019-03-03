
//#define SOCI_USE_BOOST

#include <soci.h>
#include <soci-postgresql.h>


namespace soci{

	template <typename T0, typename... Trest> struct
		type_conversion<std::tuple<T0,Trest...> >
		{
			public:
				typedef values base_type;

			private:

				template<std::size_t>
					struct int_{}; // compile-time counter

				static void from_base(base_type const & in, indicator ind, std::tuple<T0,Trest...> & out, int_<0> )
				{
					in >> std::get<0>(out);
				}

				template<std::size_t I>
					static void from_base(base_type const & in, indicator ind, std::tuple<T0,Trest...> & out, int_<I>  )
					{
						from_base( in, ind, out, int_<I-1>() );
						in >> std::get<I>(out);
					}

				static void to_base(std::tuple<T0,Trest...> const & in, base_type & out, indicator &ind, int_<0>){
					out<< std::get<0>(in);
				}

				template<std::size_t I>
					static void to_base(std::tuple<T0,Trest...> const & in, base_type & out, indicator &ind, int_<I>){
						to_base(in, out, ind, int_<I-1>());
						out<< std::get<I>(in);
					}

			public:

				static void from_base(base_type const & in, indicator ind,
						std::tuple<T0,Trest...> & out)
				{
					from_base( in, ind, out, int_<sizeof...(Trest)>() );
				}

				static void to_base(std::tuple<T0,Trest...> const & in, base_type & out, indicator &ind){

					to_base(in, out, ind, int_<sizeof...(Trest)>() );

				}
		};

}

struct database_output_iterator
{
	explicit database_output_iterator(soci::session & _sql,std::string const &_table_name, std::vector<std::string> const & _col_names, std::vector<std::string> const & _col_types):sql(_sql), table_name(_table_name), col_names(_col_names), col_types(_col_types){

		format = col_names[0];
		placeholders = ":";
		placeholders += col_names[0];
		for(size_t i=1;i<col_names.size();++i){
			format += ",";
			format += col_names[i];
			placeholders += ", :";
			placeholders += col_names[i];
		}


	}

	soci::session  &sql;
	std::string const &table_name;
	std::vector<std::string> const &col_names;
	std::vector<std::string> const &col_types;
	std::string format;
	std::string placeholders;

	void destroy(){sql<<("drop table "+table_name); }
	//bool exist_table(){}
	void create(){

		std::string stmt;
		assert(col_names.size()==col_types.size());
		assert(col_names.size()>0);


		stmt += col_names[0];
		stmt += " ";
		stmt += col_types[0];

		for(std::size_t i=1;i<col_names.size();++i){
			stmt += ",";
			stmt += col_names[i];
			stmt += " ";
			stmt += col_types[i];
		}


		sql<<"create table "+table_name+" ("+stmt+")" ;
	}

	template <class... Args>
		void insert(std::tuple<Args...> const &t){

			sql<<"insert into "+table_name + "(" + format +") values ("+placeholders+")", soci::use(t);
		}

	void clear(){
		sql<<"delete from " + table_name;
	}
	void copy_from(std::string const &realpath){
		sql<<"copy "+table_name +" ("+ format + ") from '" + realpath + "' delimiter ' ' csv";
	}
	void copy_to_pwd(){
		//sql<<"COPY proportion TO '/tmp/proportion.csv' DELIMITER ',' CSV HEADER";
		sql<<"COPY "+table_name+" TO '/tmp/"+table_name+".csv' DELIMITER ' ' CSV";
		system( ("cp '/tmp/"+table_name+".csv' .").c_str() );
	}

};

//two cases: vector to database, database to vector
//how about no table exist at database for two cases? should I create one automatically?
template<class Iter>
void batch_copy(Iter _start, Iter _end, database_output_iterator  &iter){

	//batch update
	for(auto it=_start; it!=_end; ++it)
		iter.insert(*it);

}


